/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2008 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2008 Nokia Corporation
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

#ifndef _TelepathyQt4_cli_channel_h_HEADER_GUARD_
#define _TelepathyQt4_cli_channel_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <TelepathyQt4/_gen/cli-channel.h>

#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Contact>
#include <TelepathyQt4/DBus>
#include <TelepathyQt4/DBusProxy>
#include <TelepathyQt4/OptionalInterfaceFactory>
#include <TelepathyQt4/ReadinessHelper>
#include <TelepathyQt4/ReadyObject>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/SharedPtr>

#include <QSet>
#include <QVariantMap>

namespace Telepathy
{

class Connection;
class PendingOperation;
class PendingReady;

class Channel : public StatefulDBusProxy,
                private OptionalInterfaceFactory<Channel>,
                public ReadyObject,
                public RefCounted
{
    Q_OBJECT
    Q_DISABLE_COPY(Channel)

public:
    static const Feature FeatureCore;

    static ChannelPtr create(const ConnectionPtr &connection,
            const QString &objectPath, const QVariantMap &immutableProperties);

    ~Channel();

    ConnectionPtr connection() const;

    QString channelType() const;
    QStringList interfaces() const;

    uint targetHandleType() const;
    uint targetHandle() const;

    bool isRequested() const;
    ContactPtr initiatorContact() const;

    PendingOperation *requestClose();

    uint groupFlags() const;

    bool groupCanAddContacts() const;
    PendingOperation *groupAddContacts(const QList<ContactPtr> &contacts,
            const QString &message = QString());
    bool groupCanRescindContacts() const;
    bool groupCanRemoveContacts() const;
    PendingOperation *groupRemoveContacts(const QList<ContactPtr> &contacts,
            const QString &message = QString(),
            uint reason = Telepathy::ChannelGroupChangeReasonNone);

    Contacts groupContacts() const;
    Contacts groupLocalPendingContacts() const;
    Contacts groupRemotePendingContacts() const;

    class GroupMemberChangeDetails
    {
    public:
        GroupMemberChangeDetails()
            : mIsValid(false) {}

        bool isValid() const { return mIsValid; }

        bool hasActor() const { return !mActor.isNull(); }
        ContactPtr actor() const { return mActor; }

        bool hasReason() const { return mDetails.contains("change-reason"); }
        uint reason() const { return qdbus_cast<uint>(mDetails.value("change-reason")); }

        bool hasMessage() const { return mDetails.contains("message"); }
        QString message () const { return qdbus_cast<QString>(mDetails.value("message")); }

        bool hasError() const { return mDetails.contains("error"); }
        QString error() const { return qdbus_cast<QString>(mDetails.value("error")); }

        bool hasDebugMessage() const { return mDetails.contains("debug-message"); }
        QString debugMessage() const { return qdbus_cast<QString>(mDetails.value("debug-message")); }

        QVariantMap allDetails() const { return mDetails; }

    private:
        friend class Channel;

        GroupMemberChangeDetails(const ContactPtr &actor, const QVariantMap &details)
            : mActor(actor), mDetails(details), mIsValid(true) {}

        ContactPtr mActor;
        QVariantMap mDetails;
        bool mIsValid;
    };

    GroupMemberChangeDetails groupLocalPendingContactChangeInfo(const ContactPtr &contact) const;
    GroupMemberChangeDetails groupSelfContactRemoveInfo() const;

    bool groupAreHandleOwnersAvailable() const;
    HandleOwnerMap groupHandleOwners() const;

    bool groupIsSelfContactTracked() const;
    ContactPtr groupSelfContact() const;

Q_SIGNALS:
    void groupFlagsChanged(uint flags, uint added, uint removed);

    void groupCanAddContactsChanged(bool canAddContacts);
    void groupCanRemoveContactsChanged(bool canRemoveContacts);
    void groupCanRescindContactsChanged(bool canRescindContacts);

    void groupMembersChanged(
            const Telepathy::Contacts &groupMembersAdded,
            const Telepathy::Contacts &groupLocalPendingMembersAdded,
            const Telepathy::Contacts &groupRemotePendingMembersAdded,
            const Telepathy::Contacts &groupMembersRemoved,
            const Telepathy::Channel::GroupMemberChangeDetails &details);

    void groupHandleOwnersChanged(const Telepathy::HandleOwnerMap &owners,
            const Telepathy::UIntList &added, const Telepathy::UIntList &removed);

    void groupSelfContactChanged();

protected:
    bool groupSelfHandleIsLocalPending() const;

protected Q_SLOTS:
    PendingOperation *groupAddSelfHandle();

public:
    template <class Interface>
    inline Interface *optionalInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        // Check for the remote object supporting the interface
        QString name(Interface::staticInterfaceName());
        if (check == CheckInterfaceSupported && !interfaces().contains(name))
            return 0;

        // If present or forced, delegate to OptionalInterfaceFactory
        return OptionalInterfaceFactory<Channel>::interface<Interface>();
    }

    inline Client::ChannelInterfaceCallStateInterface *callStateInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return optionalInterface<Client::ChannelInterfaceCallStateInterface>(check);
    }

    inline Client::ChannelInterfaceChatStateInterface *chatStateInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return optionalInterface<Client::ChannelInterfaceChatStateInterface>(check);
    }

    inline Client::ChannelInterfaceDTMFInterface *DTMFInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return optionalInterface<Client::ChannelInterfaceDTMFInterface>(check);
    }

    inline Client::ChannelInterfaceHoldInterface *holdInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return optionalInterface<Client::ChannelInterfaceHoldInterface>(check);
    }

    inline Client::ChannelInterfaceMediaSignallingInterface *mediaSignallingInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return optionalInterface<Client::ChannelInterfaceMediaSignallingInterface>(check);
    }

    inline Client::ChannelInterfaceMessagesInterface *messagesInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return optionalInterface<Client::ChannelInterfaceMessagesInterface>(check);
    }

    inline Client::ChannelInterfacePasswordInterface *passwordInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return optionalInterface<Client::ChannelInterfacePasswordInterface>(check);
    }

    inline Client::DBus::PropertiesInterface *propertiesInterface() const
    {
        return optionalInterface<Client::DBus::PropertiesInterface>(BypassInterfaceCheck);
    }

    template <class Interface>
    inline Interface *typeInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        // Check for the remote object having the correct channel type
        QString name(Interface::staticInterfaceName());
        if (check == CheckInterfaceSupported && channelType() != name)
            return 0;

        // If correct type or check bypassed, delegate to OIF
        return OptionalInterfaceFactory<Channel>::interface<Interface>();
    }

    inline Client::ChannelTypeRoomListInterface *roomListInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return typeInterface<Client::ChannelTypeRoomListInterface>(check);
    }

    inline Client::ChannelTypeStreamedMediaInterface *streamedMediaInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return typeInterface<Client::ChannelTypeStreamedMediaInterface>(check);
    }

    inline Client::ChannelTypeTextInterface *textInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return typeInterface<Client::ChannelTypeTextInterface>(check);
    }

    inline Client::ChannelTypeTubesInterface *tubesInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return typeInterface<Client::ChannelTypeTubesInterface>(check);
    }

protected:
    Channel(const ConnectionPtr &connection,const QString &objectPath,
            const QVariantMap &immutableProperties);

    Client::ChannelInterface *baseInterface() const;

    inline Client::ChannelInterfaceGroupInterface *groupInterface(
            InterfaceSupportedChecking check = CheckInterfaceSupported) const
    {
        return optionalInterface<Client::ChannelInterfaceGroupInterface>(check);
    }

private Q_SLOTS:
    void gotMainProperties(QDBusPendingCallWatcher *watcher);
    void gotChannelType(QDBusPendingCallWatcher *watcher);
    void gotHandle(QDBusPendingCallWatcher *watcher);
    void gotInterfaces(QDBusPendingCallWatcher *watcher);
    void onClosed();

    void onConnectionInvalidated();
    void onConnectionDestroyed();

    void gotGroupProperties(QDBusPendingCallWatcher *watcher);
    void gotGroupFlags(QDBusPendingCallWatcher *watcher);
    void gotAllMembers(QDBusPendingCallWatcher *watcher);
    void gotLocalPendingMembersWithInfo(QDBusPendingCallWatcher *watcher);
    void gotSelfHandle(QDBusPendingCallWatcher *watcher);
    void gotContacts(Telepathy::PendingOperation *op);

    void onGroupFlagsChanged(uint, uint);
    void onMembersChanged(const QString&,
            const Telepathy::UIntList&, const Telepathy::UIntList&,
            const Telepathy::UIntList&, const Telepathy::UIntList&, uint, uint);
    void onMembersChangedDetailed(
        const Telepathy::UIntList &added, const Telepathy::UIntList &removed,
        const Telepathy::UIntList &localPending, const Telepathy::UIntList &remotePending,
        const QVariantMap &details);
    void onHandleOwnersChanged(const Telepathy::HandleOwnerMap&, const Telepathy::UIntList&);
    void onSelfHandleChanged(uint);

    void continueIntrospection();

private:
    struct Private;
    friend struct Private;
    Private *mPriv;
};

} // Telepathy

Q_DECLARE_METATYPE(Telepathy::Channel::GroupMemberChangeDetails);

#endif
