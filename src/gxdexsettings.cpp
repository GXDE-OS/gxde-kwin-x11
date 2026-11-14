/*
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "gxdexsettings.h"

#include "main.h"
#include "utils/xcbutils.h"

#include <KSelectionWatcher>

#include <QColor>
#include <QtEndian>

namespace KWin
{

namespace
{
enum XSettingsType {
    XSettingsTypeInteger = 0,
    XSettingsTypeString = 1,
    XSettingsTypeColor = 2,
};

class XSettingsReader
{
public:
    explicit XSettingsReader(const QByteArray &data)
        : m_data(data)
        , m_littleEndian(!data.isEmpty() && data.at(0) == XCB_IMAGE_ORDER_LSB_FIRST)
    {
    }

    bool skip(qsizetype length)
    {
        if (length < 0 || m_offset + length > m_data.size()) {
            return false;
        }
        m_offset += length;
        return true;
    }

    template<typename T>
    bool read(T &value)
    {
        if (m_offset + qsizetype(sizeof(T)) > m_data.size()) {
            return false;
        }
        const uchar *p = reinterpret_cast<const uchar *>(m_data.constData() + m_offset);
        value = m_littleEndian ? qFromLittleEndian<T>(p) : qFromBigEndian<T>(p);
        m_offset += sizeof(T);
        return true;
    }

    bool readBytes(qsizetype length, QByteArray &bytes)
    {
        if (length < 0 || m_offset + length > m_data.size()) {
            return false;
        }
        bytes = m_data.mid(m_offset, length);
        // Names and strings are padded to a multiple of four bytes.
        return skip(((length + 3) & ~3));
    }

private:
    const QByteArray &m_data;
    const bool m_littleEndian;
    qsizetype m_offset = 0;
};
} // namespace

QVariantMap parseXSettings(const QByteArray &data)
{
    QVariantMap settings;
    if (data.size() < 12 || (data.at(0) != XCB_IMAGE_ORDER_LSB_FIRST && data.at(0) != XCB_IMAGE_ORDER_MSB_FIRST)) {
        return settings;
    }

    XSettingsReader reader(data);
    quint32 serial = 0;
    quint32 count = 0;
    if (!reader.skip(4) || !reader.read(serial) || !reader.read(count)) {
        return settings;
    }

    for (quint32 i = 0; i < count; ++i) {
        quint8 type = 0;
        quint16 nameLength = 0;
        QByteArray name;
        quint32 lastChangeSerial = 0;
        if (!reader.read(type) || !reader.skip(1) || !reader.read(nameLength)
            || !reader.readBytes(nameLength, name) || !reader.read(lastChangeSerial)) {
            break;
        }

        if (type == XSettingsTypeInteger) {
            qint32 value = 0;
            if (!reader.read(value)) {
                break;
            }
            settings.insert(QString::fromLatin1(name), value);
        } else if (type == XSettingsTypeString) {
            quint32 length = 0;
            QByteArray value;
            if (!reader.read(length) || !reader.readBytes(length, value)) {
                break;
            }
            settings.insert(QString::fromLatin1(name), value);
        } else if (type == XSettingsTypeColor) {
            quint16 red = 0, green = 0, blue = 0, alpha = 0;
            if (!reader.read(red) || !reader.read(green) || !reader.read(blue) || !reader.read(alpha)) {
                break;
            }
            // The specification uses 16 bit channels; dxcb writes 8 bit values.
            const bool wide = red > 255 || green > 255 || blue > 255 || alpha > 255;
            settings.insert(QString::fromLatin1(name),
                            wide ? QColor::fromRgba64(red, green, blue, alpha) : QColor(red, green, blue, alpha));
        } else {
            break;
        }
    }

    return settings;
}

GxdeXSettingsWatcher::GxdeXSettingsWatcher(QObject *parent)
    : QObject(parent)
    , X11EventFilter(XCB_PROPERTY_NOTIFY)
{
    xcb_connection_t *connection = kwinApp()->x11Connection();
    m_settingsAtom = Xcb::Atom(QByteArrayLiteral("_XSETTINGS_SETTINGS"), false, connection);

    const Xcb::Atom selection(QByteArrayLiteral("_XSETTINGS_S0"), false, connection);
    m_selectionWatcher = std::make_unique<KSelectionWatcher>(selection, connection, kwinApp()->x11RootWindow());
    connect(m_selectionWatcher.get(), &KSelectionWatcher::newOwner, this, &GxdeXSettingsWatcher::setOwner);
    connect(m_selectionWatcher.get(), &KSelectionWatcher::lostOwner, this, [this]() {
        setOwner(XCB_WINDOW_NONE);
    });
    setOwner(m_selectionWatcher->owner());
}

GxdeXSettingsWatcher::~GxdeXSettingsWatcher() = default;

QVariant GxdeXSettingsWatcher::value(const QByteArray &name) const
{
    return m_settings.value(QString::fromLatin1(name));
}

void GxdeXSettingsWatcher::setOwner(xcb_window_t owner)
{
    if (m_owner == owner) {
        return;
    }
    m_owner = owner;
    if (m_owner != XCB_WINDOW_NONE) {
        const uint32_t values[] = {XCB_EVENT_MASK_PROPERTY_CHANGE};
        xcb_change_window_attributes(kwinApp()->x11Connection(), m_owner, XCB_CW_EVENT_MASK, values);
    }
    readSettings();
}

void GxdeXSettingsWatcher::readSettings()
{
    QVariantMap settings;
    if (m_owner != XCB_WINDOW_NONE) {
        Xcb::Property property(false, m_owner, m_settingsAtom, m_settingsAtom, 0, 65536);
        settings = parseXSettings(property.toByteArray(8, m_settingsAtom));
    }
    if (settings != m_settings) {
        m_settings = settings;
        Q_EMIT changed();
    }
}

bool GxdeXSettingsWatcher::event(xcb_generic_event_t *event)
{
    const auto *pe = reinterpret_cast<xcb_property_notify_event_t *>(event);
    if (m_owner != XCB_WINDOW_NONE && pe->window == m_owner && pe->atom == m_settingsAtom) {
        readSettings();
    }
    return false;
}

} // namespace KWin

#include "moc_gxdexsettings.cpp"
