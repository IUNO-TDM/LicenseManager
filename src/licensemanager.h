#ifndef SRC_LICENSEMANAGER_H_
#define SRC_LICENSEMANAGER_H_

#include "licensemanagerinterface.h"
#include "CodeMeter.h"

class LicenseManager: public LicenseManagerInterface {
    public:
        virtual ~LicenseManager(){}

        virtual void GetDongles(std::vector<std::string>& dongles);
        virtual void GetContext(const std::string& dongle_id, std::string& context);
        virtual void Update(const std::string& dongle_id, const std::string& rau_data);
        virtual size_t GetLicenseCount(const std::string& dongle_id, const std::string& product_id);
        virtual void GetLicenses(const std::string& dongle_id, std::map<unsigned long, unsigned long>& licenses);

    private:
        static std::string GetLastErrorText();
        static void SplitDongleId(const std::string& dongle_id, CMUSHORT& mask, CMULONG& serial);
        static void GetLicenses(const std::string& dongle_id, std::map<unsigned long, unsigned long>& licenses, const std::string& log_prefix);
};

#endif
