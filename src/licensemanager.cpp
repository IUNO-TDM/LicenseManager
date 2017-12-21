#include "licensemanager.h"
#include "cryptohelpers.h"

#include "easylogging++.h"

#include <boost/regex.hpp>
#include <stdlib.h>

using namespace std;

void LicenseManager::GetDongles(vector<string>& dongles){
    LOG(DEBUG)<< "Getting dongles.";

    CMACCESS2 cmAcc;
    memset(&cmAcc, 0, sizeof(cmAcc));
    cmAcc.mflCtrl |= CM_ACCESS_NOUSERLIMIT;
    HCMSysEntry hcmse = CmAccess2(CM_ACCESS_LOCAL, &cmAcc);
    if(hcmse){
        const unsigned int cbBoxInfo = 64; // we won't ever plug more than 64 dongles ...
        CMBOXINFO cmBoxInfo[cbBoxInfo];
        int found = CmGetBoxes(hcmse, CM_GB_DONGLE, cmBoxInfo, cbBoxInfo);
        string dongles_log;
        for(int i =0; i<found; i++)
        {
            stringstream ss;
            ss << cmBoxInfo[i].musBoxMask << "-" << cmBoxInfo[i].mulSerialNumber;
            dongles.push_back(ss.str());
            if(dongles_log != ""){
                dongles_log+=",";
            }
            dongles_log += ss.str();
        }

        LOG(DEBUG)<< "Dongles successfully got: [" << dongles_log << "].";

        if(!CmRelease(hcmse)){
            LOG(ERROR)<< "CmRelease failed.";
        }

    }else{
        stringstream ss;
        ss << "Getting dongles failed. Reason: '" << GetLastErrorText() << "'.";
        LOG(ERROR) << ss.str();
        throw invalid_argument(ss.str());
    }
}

void LicenseManager::GetContext(const string& dongle_id, string& context){
    LOG(DEBUG)<< "Getting context for dongle '" << dongle_id << "'.";

    CMACCESS2 cmAcc;
    memset(&cmAcc, 0, sizeof(cmAcc));
    cmAcc.mflCtrl |= CM_ACCESS_NOUSERLIMIT;
    SplitDongleId(dongle_id, cmAcc.musBoxMask, cmAcc.mulSerialNumber);

    HCMSysEntry hcmse = CmAccess2(CM_ACCESS_LOCAL, &cmAcc);
    if(hcmse)
    {
        CMLTREQUEST cmLtRequest;
        memset(&cmLtRequest, 0, sizeof(cmLtRequest));
        cmLtRequest.mulFirmCode = 6000274;
        const size_t buffer_size = 256*1024;
        unsigned char buffer[buffer_size];
        memset(buffer, 0, buffer_size);
        int res = CmLtCreateContext(hcmse, CM_LT_FSB, &cmLtRequest, NULL, 0, buffer, buffer_size);
        if(res){
            if(res < static_cast<int>(buffer_size)){
                CryptoBuffer cb;
                cb.set(buffer, res);
                CryptoHelpers::Base64(cb, context);
                LOG(DEBUG)<< "Context successfully got.";
            }else{
                stringstream ss;
                ss << "Getting context for dongle '" << dongle_id << "' failed due to an insufficient buffer size.";
                LOG(ERROR) << ss.str();
                throw runtime_error(ss.str());
            }
        }else{
            stringstream ss;
            ss << "Getting context for dongle '" << dongle_id << "' failed. Reason: '" << GetLastErrorText() << "'.";
            LOG(ERROR) << ss.str();
            throw runtime_error(ss.str());
        }

        if(!CmRelease(hcmse)){
            LOG(ERROR)<< "CmRelease failed.";
        }
    }else{
        stringstream ss;
        ss << "Getting context for dongle '" << dongle_id << "' failed. Reason: '" << GetLastErrorText() << "'.";
        LOG(ERROR) << ss.str();
        throw invalid_argument(ss.str());
    }
}

void LicenseManager::Update(const string& dongle_id, const string& rau_data){
    LOG(DEBUG)<< "Updating dongle '" << dongle_id << "'.";

    CryptoBuffer rau_content;
    CryptoHelpers::Unbase64(rau_data, rau_content);

    CMACCESS2 cmAcc;
    memset(&cmAcc, 0, sizeof(cmAcc));
    cmAcc.mflCtrl |= CM_ACCESS_NOUSERLIMIT;
    SplitDongleId(dongle_id, cmAcc.musBoxMask, cmAcc.mulSerialNumber);

    HCMSysEntry hcmse = CmAccess2(CM_ACCESS_LOCAL, &cmAcc);
    if(hcmse){
        int res = CmLtImportUpdate(hcmse, CM_LT_PUSH, rau_content, rau_content.size());
        if(res){
            LOG(DEBUG)<< "Dongle successfully updated.";
        }else{
            stringstream ss;
            ss << "Updating dongle '" << dongle_id << "' failed. Reason: '" << GetLastErrorText() << "'.";
            LOG(ERROR) << ss.str();
            throw runtime_error(ss.str());
        }

        if(!CmRelease(hcmse)){
            LOG(WARNING)<< "CmRelease failed.";
        }
    }else{
        stringstream ss;
        ss << "Updating dongle '" << dongle_id << "' failed. Reason: '" << GetLastErrorText() << "'.";
        LOG(ERROR) << ss.str();
        throw invalid_argument(ss.str());
    }
}

void LicenseManager::GetLicenses(const string& dongle_id, map<unsigned long, unsigned long>& licenses, const string& log_prefix){
    CMACCESS2 access;
    memset(&access, 0, sizeof(access));
    access.mflCtrl |= CM_ACCESS_NOUSERLIMIT;

    bool query_specific_dongle = false;
    if(dongle_id != "all"){
        query_specific_dongle = true;
        SplitDongleId(dongle_id, access.musBoxMask, access.mulSerialNumber);
    }

    HCMSysEntry hcmse = CmAccess2(CM_ACCESS_LOCAL, &access);
    if(hcmse){
        CMBOXINFO box_info;
        memset(&box_info, 0, sizeof(box_info));
        box_info.musBoxMask = access.musBoxMask;
        box_info.mulSerialNumber = access.mulSerialNumber;
        int res = CmGetBoxContents2(hcmse, (query_specific_dongle?CM_GBC_BOX:0) | CM_GBC_FI, 6000274, &box_info, NULL, 0);
        unsigned box_entry_count = res;
        CMBOXENTRY2* box_entries = NULL;
        if(res>0){
            box_entries = new CMBOXENTRY2[box_entry_count];
            res = CmGetBoxContents2(hcmse, (query_specific_dongle?CM_GBC_BOX:0) | CM_GBC_FI, 6000274, &box_info, box_entries, box_entry_count);
        }
        if(res>0){
            if(static_cast<unsigned>(res) != box_entry_count){
                stringstream ss;
                ss << log_prefix << " failed due to an internal error.";
                LOG(ERROR) << ss.str();
                if(!CmRelease(hcmse)){
                    LOG(WARNING)<< "CmRelease failed.";
                }
                delete[] box_entries;
                throw runtime_error(ss.str());
            }
            for(unsigned i = 0; i < box_entry_count; i++){
                unsigned long product_id = box_entries[i].mulProductCode;
                if(licenses.count(product_id)>0){
                    licenses[product_id] += box_entries[i].mulUnitCounter;
                }else{
                    licenses[product_id] = box_entries[i].mulUnitCounter;
                }
            }
        }else{
            if(CmGetLastErrorCode() != CMERROR_ENTRY_NOT_FOUND){
                stringstream ss;
                ss << log_prefix << " failed. Reason: '" << GetLastErrorText() << "'.";
                LOG(ERROR) << ss.str();
                if(!CmRelease(hcmse)){
                    LOG(WARNING)<< "CmRelease failed.";
                }
                delete[] box_entries;
                throw runtime_error(ss.str());
            }
        }
        delete[] box_entries;

        if(!CmRelease(hcmse)){
            LOG(WARNING)<< "CmRelease failed.";
        }
    }else{
        stringstream ss;
        ss << log_prefix << " failed. Reason: '" << GetLastErrorText() << "'.";
        LOG(ERROR) << ss.str();
        throw invalid_argument(ss.str());
    }
}

void LicenseManager::GetLicenses(const string& dongle_id, map<unsigned long, unsigned long>& licenses){
    stringstream ss;
    ss << "Getting licenses for dongle '" << dongle_id << "'";
    string log_prefix = ss.str();

    LOG(DEBUG)<< log_prefix << "'.";

    GetLicenses(dongle_id, licenses, log_prefix);

    LOG(DEBUG) << log_prefix << " succeeded.";
}


size_t LicenseManager::GetLicenseCount(const string& dongle_id, const string& product_id){
    stringstream ss;
    ss << "Getting license count for dongle '" << dongle_id << "' and product id '" << product_id << "'";
    string log_prefix = ss.str();

    LOG(DEBUG)<< log_prefix << "'.";

    bool product_id_good = false;
    boost::smatch what;
    unsigned long long product_id_ull = 0;
    if(boost::regex_search(product_id, what, boost::regex("^([0-9]{1,10})$"))){
        product_id_ull = strtoull(what[1].str().c_str(), NULL, 10);
        if(product_id_ull <= 0xffffffff){
            product_id_good = true;
        }
    }
    if(!product_id_good){
        stringstream ss;
        ss << log_prefix << " failed because product id is out of range.";
        LOG(ERROR) << ss.str();
        throw invalid_argument(ss.str());
    }
    unsigned long product_id_ul = static_cast<unsigned long>(product_id_ull);

    size_t license_count = 0;

    map<unsigned long, unsigned long> licenses;
    GetLicenses(dongle_id, licenses, log_prefix);

    for(auto& i :licenses){
        if(i.first==product_id_ul){
            license_count = i.second;
            break;
        }
    }

    LOG(DEBUG) << log_prefix << " succeeded with " << license_count << " licenses found.";

    return license_count;
}

string LicenseManager::GetLastErrorText(){
    string error_text;
    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);

    unsigned int res = CmGetLastErrorText(CM_GLET_ERRORTEXT | CM_GLET_UTF8_ENCODING, buffer, buffer_size);
    if(res){
        if(res <= buffer_size){
            error_text = buffer;
        } else {
            error_text = "Insufficient buffer for getting the error description.";
        }
    } else {
        error_text = "Unknown error.";
    }
    return error_text;
}

void LicenseManager::SplitDongleId(const string& dongle_id, CMUSHORT& mask, CMULONG& serial){
    bool good = false;
    boost::smatch what;
    if(boost::regex_search(dongle_id, what, boost::regex("^([0-9]{1,5})-([0-9]{1,10})$"))){
        unsigned long long ullmask = strtoull(what[1].str().c_str(), NULL, 10);
        unsigned long long ullserial = strtoull(what[2].str().c_str(), NULL, 10);
        if((ullmask<0xffff) && (ullserial<0xffffffff)) {
            mask = ullmask;
            serial = ullserial;
            good = true;
        }
    }
    if(!good){
        stringstream ss;
        ss << "Dongle id '" << dongle_id << "' is invalid.";
        LOG(ERROR) << ss.str();
        throw invalid_argument(ss.str());
    }

}


