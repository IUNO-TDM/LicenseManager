#ifndef SRC_LICENSEMANAGERINTERFACE_H_
#define SRC_LICENSEMANAGERINTERFACE_H_

#include <vector>
#include <string>

class LicenseManagerInterface {
    public:
        virtual ~LicenseManagerInterface(){}
        virtual void GetDongles(std::vector<std::string>& dongles) = 0;
        virtual void GetContext(const std::string& dongle_id, std::string& context) = 0;
        virtual void Update(const std::string& dongle_id, const std::string& rau_data) = 0;
        virtual size_t GetLicenseCount(const std::string& dongle_id, const std::string& product_id) = 0;
};

#endif
