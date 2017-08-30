#ifndef SRC_LICENSEMANAGER_H_
#define SRC_LICENSEMANAGER_H_

#include "licensemanagerinterface.h"

class LicenseManager: public LicenseManagerInterface {
    public:
        LicenseManager();
        virtual ~LicenseManager();

        virtual void GetDongles(std::vector<std::string>& dongles);
        virtual void GetContext(const std::string& dongle_id, std::string& context);
        virtual void Update(const std::string& dongle_id, const std::string& rau_data);
        virtual size_t GetLicenseCount(const std::string& dongle_id, const std::string& product_id);
};

#endif
