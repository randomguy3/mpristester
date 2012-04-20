/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Alex Merry <dev@randomguy3.me.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef INTERFACETEST_H
#define INTERFACETEST_H

class QDBusInterface;
class QDBusVariant;
class QDBusMessage;
class QTimer;
#include <QObject>
#include <QStringList>
#include <QVariantMap>

namespace Mpris2
{
    class InterfaceTest : public QObject
    {
        Q_OBJECT

    public:
        enum LocationType {
            Property,
            Method,
            Signal,
            Other
        };

        QVariantMap properties() const;

    public slots:
        /**
            * Performs a basic initial test.
            *
            * Checks for property existence and types, mainly.
            */
        void initialTest();

        /**
            * Performs an incremental test.
            *
            * Checks that if properties have changed, we have
            * been notified (actually waits a couple of seconds for
            * slightly delayed notifications before complaining).
            */
        void incrementalTest();

    protected:
        InterfaceTest(const QString& interface, const QString& service, QObject* parent);
        virtual ~InterfaceTest();

    signals:
        /**
          * Reports that there was a problem with the interface.
          *
          * Indicates that a violation of MPRIS has been found.
          *
          * @param desc  a user-readable description of the error
          */
        void interfaceError(Mpris2::InterfaceTest::LocationType locType, const QString& location, const QString& desc);

        /**
          * Reports that there might be a problem with the interface.
          *
          * This is something that isn't necessarily a problem, but
          * probably is, like not including the "file" protocol
          * in SupportedUriSchemes.
          *
          * @param desc  a user-readable description of the warning
          */
        void interfaceWarning(Mpris2::InterfaceTest::LocationType locType, const QString& location, const QString& desc);

        /**
          * Reports salient information about the testing.
          *
          * Provides useful information for developers about their
          * implementation, allowing them to ensure that it is
          * reporting what they expect.
          *
          * This will generally be either things that could be a
          * problem, but probably aren't, or indicate that the user
          * needs to check something worked as expected (because it
          * can't be checked directly in the interface).
          */
        void interfaceInfo(Mpris2::InterfaceTest::LocationType locType, const QString& location, const QString& desc);

        /**
         * Reports that some properties have changed.
         *
         * This is tied to the interface's propertiesChanged signal,
         * and so will only be emitted for property changes reported
         * using that mechanism.
         */
        void propertiesChanged(const QStringList& properties);

    private slots:
        void _m_propertiesChanged(const QString& interface,
                                  const QVariantMap& changedProperties,
                                  const QStringList& invalidatedProperties,
                                  const QDBusMessage& signalMessage);
        void delayedIncrementalCheck();

    protected:
        enum PropErrorAllowance {
            PropDisallowErrors = 0,
            PropAllowMissing = 1,
            PropAllowReadOnly = 2,
            PropAllowErrors = PropAllowMissing | PropAllowReadOnly
        };
        bool getAllProps();
        bool getProp(const QString& propName,
                     PropErrorAllowance allowError = PropDisallowErrors);
        bool setProp(const QString& propName,
                     const QDBusVariant& value,
                     PropErrorAllowance allowError = PropAllowReadOnly);

        bool checkPropValid(const QString& propName,
                            QVariant::Type expType,
                            const QVariantMap& oldProps = QVariantMap());
        bool checkOptionalPropValid(const QString& propName,
                            QVariant::Type expType,
                            const QVariantMap& oldProps = QVariantMap());
        bool checkNonEmptyStringPropValid(const QString& propName,
                                          const QVariantMap& oldProps = QVariantMap());
        bool checkOptionalNonEmptyStringPropValid(const QString& propName,
                                          const QVariantMap& oldProps = QVariantMap());
        void checkMetadata(const QVariantMap& metadata,
                           QStringList* errors,
                           QStringList* warnings,
                           QStringList* infoMessages);

        virtual void checkProps(const QVariantMap& oldProps = QVariantMap()) = 0;
        virtual void checkUpdatedProperty(const QString& propName) = 0;
        virtual void checkConsistency(const QVariantMap& oldProps = QVariantMap()) = 0;
        virtual void connectSignals();

        QDBusInterface* iface;
        QVariantMap     props;
        QVariantMap     outOfDateProperties; // prop name -> new value
        QStringList     propsNotUpdated;

    private:
        bool checkMetadataEntry(const QVariantMap& metadata,
                                const QString& entry,
                                QVariant::Type type,
                                QStringList* errors,
                                QStringList* warnings,
                                QStringList* infoMessages);

        QDBusInterface*    propsIface;
        QTimer*            delayedCheckTimer;
        QMap<QString,uint> propertyUpdateWarningCount;
    };
}

#endif // INTERFACETEST_H
// vim:et:sw=4:sts=4
