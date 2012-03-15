/**
 * This file is part of TelepathyQt
 *
 * @copyright Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * @copyright Copyright (C) 2010 Nokia Corporation
 * @license LGPL 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <TelepathyQt/DBusError>

#include <QString>

namespace Tp
{

struct TP_QT_NO_EXPORT DBusError::Private
{
    Private(const QString &name, const QString &message)
        : name(name),
          message(message)
    {
    }

    QString name;
    QString message;
};

DBusError::DBusError()
    : mPriv(0)
{
}

DBusError::DBusError(const QString &name, const QString &message)
    : mPriv(new Private(name, message))
{
}

DBusError::~DBusError()
{
}

bool DBusError::operator==(const DBusError &other) const
{
    if (!isValid() || !other.isValid()) {
        if (!isValid() && !other.isValid()) {
            return true;
        }
        return false;
    }

    return mPriv->name == other.mPriv->name &&
        mPriv->message == other.mPriv->message;
}

bool DBusError::operator!=(const DBusError &other) const
{
    if (!isValid() || !other.isValid()) {
        if (!isValid() && !other.isValid()) {
            return false;
        }
        return true;
    }

    return mPriv->name != other.mPriv->name ||
        mPriv->message != other.mPriv->message;
}

QString DBusError::name() const
{
    if (!isValid()) {
        return QString();
    }

    return mPriv->name;
}

QString DBusError::message() const
{
    if (!isValid()) {
        return QString();
    }

    return mPriv->message;
}

void DBusError::set(const QString &name, const QString &message)
{
    if (!isValid()) {
        mPriv = new Private(name, message);
        return;
    }

    mPriv->name = name;
    mPriv->message = message;
}

} // Tp
