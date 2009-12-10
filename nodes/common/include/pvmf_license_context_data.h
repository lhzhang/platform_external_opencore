/* ------------------------------------------------------------------
 * Copyright (C) 1998-2009 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */
#ifndef PVMF_LICENSE_CONTEXT_DATA_H_INCLUDED
#define PVMF_LICENSE_CONTEXT_DATA_H_INCLUDED

#ifndef OSCL_BASE_H_INCLUDED
#include "oscl_base.h"
#endif
#ifndef OSCL_STRING_H_INCLUDED
#include "oscl_string.h"
#endif
#ifndef OSCL_STRING_CONTAINERS_H_INCLUDED
#include "oscl_string_containers.h"
#endif
#ifndef PV_UUID_H_INCLUDED
#include "pv_uuid.h"
#endif
#ifndef PV_INTERFACE_H_INCLUDED
#include "pv_interface.h"
#endif
#ifndef PVMF_CPMPLUGIN_DOMAIN_INTERFACE_TYPES_H_INCLUDED
#include "pvmf_cpmplugin_domain_interface_types.h"
#endif
#define PVMF_LICENSE_CONTEXT_DATA_UUID PVUuid(0x13f2d930,0x8f58,0x11de,0x8a,0x39,0x08,0x00,0x20,0x0c,0x9a,0x66)
#define PVMF_DOMAIN_LICENSE_CONTEXT_DATA_UUID PVUuid(0x574a8890,0x8f58,0x11de,0x8a,0x39,0x08,0x00,0x20,0x0c,0x9a,0x66)
#define PVMF_HEADER_LICENSE_CONTEXT_DATA_UUID PVUuid(0x15806e40,0xcfa7,0x11de,0x8a,0x39,0x08,0x00,0x20,0x0c,0x9a,0x66)

class PVMFDomainLicenseDataSource : public PVInterface
{
    public:
        PVMFDomainLicenseDataSource()
        {
            iRefCounter = 0;
        };

        PVMFDomainLicenseDataSource(const PVMFDomainLicenseDataSource& aSrc) : PVInterface(aSrc)
        {
            iRefCounter = 0;
            MyCopy(aSrc);
        };

        PVMFDomainLicenseDataSource& operator=(const PVMFDomainLicenseDataSource& aSrc)
        {
            if (&aSrc != this)
            {
                MyCopy(aSrc);
            }
            return *this;
        };

        /* From PVInterface */
        void addRef()
        {
            iRefCounter++;
        }
        void removeRef()
        {
            iRefCounter--;
        }
        bool queryInterface(const PVUuid& uuid, PVInterface*& iface)
        {
            if (uuid == PVUuid(PVMF_DOMAIN_LICENSE_CONTEXT_DATA_UUID))
            {
                iface = this;
                return true;
            }
            else
            {
                iface = NULL;
                return false;
            }
        }

        PVMFCPMDomainId iDomainId;
    private:
        void MyCopy(const PVMFDomainLicenseDataSource& aSrc)
        {
            iDomainId = aSrc.iDomainId;
        };

        int32 iRefCounter;

};

class PVMFHeaderLicenseDataSource : public PVInterface
{
    public:
        PVMFHeaderLicenseDataSource()
        {
            iRefCounter = 0;
        };

        PVMFHeaderLicenseDataSource(const PVMFHeaderLicenseDataSource& aSrc) : PVInterface(aSrc)
        {
            iRefCounter = 0;
            MyCopy(aSrc);
        };

        PVMFHeaderLicenseDataSource& operator=(const PVMFHeaderLicenseDataSource& aSrc)
        {
            if (&aSrc != this)
            {
                MyCopy(aSrc);
            }
            return *this;
        };

        /* From PVInterface */
        void addRef()
        {
            iRefCounter++;
        }
        void removeRef()
        {
            iRefCounter--;
        }
        bool queryInterface(const PVUuid& uuid, PVInterface*& iface)
        {
            if (uuid == PVUuid(PVMF_HEADER_LICENSE_CONTEXT_DATA_UUID))
            {
                iface = this;
                return true;
            }
            else
            {
                iface = NULL;
                return false;
            }
        }

        OSCL_wHeapString<OsclMemAllocator> iHeader;
    private:
        void MyCopy(const PVMFHeaderLicenseDataSource& aSrc)
        {
            iHeader = aSrc.iHeader;
        };

        int32 iRefCounter;
};

class PVMFLicenseContextData : public PVInterface
{
    public:
        PVMFLicenseContextData()
        {
            iRefCounter = 0;
            iDomainLicenseContextValid = false;
            iHeaderLicenseContextValid = false;
        };

        PVMFLicenseContextData(const PVMFLicenseContextData& aSrc) : PVInterface(aSrc)
        {
            iRefCounter = 0;
            MyCopy(aSrc);
        };

        PVMFLicenseContextData& operator=(const PVMFLicenseContextData& aSrc)
        {
            if (&aSrc != this)
            {
                MyCopy(aSrc);
            }
            return *this;
        };

        /* From PVInterface */
        void addRef()
        {
            iRefCounter++;
        }
        void removeRef()
        {
            iRefCounter--;
        }
        bool queryInterface(const PVUuid& uuid, PVInterface*& iface)
        {
            if (uuid == PVUuid(PVMF_LICENSE_CONTEXT_DATA_UUID))
            {
                iface = this;
                return true;
            }
            else if (uuid == PVUuid(PVMF_DOMAIN_LICENSE_CONTEXT_DATA_UUID))
            {
                if (iDomainLicenseContextValid == true)
                {
                    iface = &iPVMFDomainLicenseDataSource;
                    return true;
                }
            }
            else if (uuid == PVUuid(PVMF_HEADER_LICENSE_CONTEXT_DATA_UUID))
            {
                if (iHeaderLicenseContextValid == true)
                {
                    iface = &iPVMFHeaderLicenseDataSource;
                    return true;
                }
            }
            iface = NULL;
            return false;
        }

        void EnableDomainLicenseContext()
        {
            iDomainLicenseContextValid = true;
        }
        void DisableDomainLicenseContext()
        {
            iDomainLicenseContextValid = false;
        }
        PVMFDomainLicenseDataSource* DomainLicense()
        {
            return iDomainLicenseContextValid ? &iPVMFDomainLicenseDataSource : NULL;
        }
        void EnableHeaderLicenseContext()
        {
            iHeaderLicenseContextValid = true;
        }
        void DisableHeaderLicenseContext()
        {
            iHeaderLicenseContextValid = false;
        }
        PVMFHeaderLicenseDataSource* HeaderLicense()
        {
            return iHeaderLicenseContextValid ? &iPVMFHeaderLicenseDataSource : NULL;
        }
        OSCL_HeapString<OsclMemAllocator> iCustomData;
    private:
        int32 iRefCounter;
        bool iDomainLicenseContextValid;
        bool iHeaderLicenseContextValid;

        PVMFDomainLicenseDataSource iPVMFDomainLicenseDataSource;
        PVMFHeaderLicenseDataSource iPVMFHeaderLicenseDataSource;

        void MyCopy(const PVMFLicenseContextData& aSrc)
        {
            iDomainLicenseContextValid = aSrc.iDomainLicenseContextValid;
            iHeaderLicenseContextValid = aSrc.iHeaderLicenseContextValid;
            iPVMFDomainLicenseDataSource = aSrc.iPVMFDomainLicenseDataSource;
            iPVMFHeaderLicenseDataSource = aSrc.iPVMFHeaderLicenseDataSource;
            iCustomData = aSrc.iCustomData;
        };
};

#endif //PVMF_LICENSE_CONTEXT_DATA_H_INCLUDED
