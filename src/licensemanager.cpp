#include "licensemanager.h"

#include "easylogging++.h"

#include "CodeMeter.h"

#include <stdlib.h>

using namespace std;

LicenseManager::LicenseManager(){
}

LicenseManager::~LicenseManager(){
}

void LicenseManager::GetDongles(vector<string>& dongles){
    LOG(DEBUG)<< "Getting dongles.";

    CMACCESS2 cmAcc;
    memset(&cmAcc, 0, sizeof(cmAcc));
    cmAcc.mflCtrl |= CM_ACCESS_NOUSERLIMIT;

    HCMSysEntry hcmse2 = CmAccess2(CM_ACCESS_LOCAL, &cmAcc);
    if(hcmse2){
        const unsigned int cbBoxInfo = 64; // we won't ever plug more than 64 dongles ...
        CMBOXINFO cmBoxInfo[cbBoxInfo];
        int found = CmGetBoxes(hcmse2, CM_GB_DONGLE, cmBoxInfo, cbBoxInfo);
        for(int i =0; i<found; i++)
        {
            stringstream ss;
            ss << cmBoxInfo[i].mulSerialNumber;
            dongles.push_back(ss.str());
        }

        LOG(DEBUG)<< "Dongles successfully got.";

        if(!CmRelease(hcmse2)){
            LOG(ERROR)<< "CmRelease failed.";
        }

    }else{
        LOG(ERROR)<< "Getting dongles failed, ???";
    }

}

void LicenseManager::GetContext(const string& dongle_id, string& context){
    LOG(DEBUG)<< "Getting context for dongle '" << dongle_id << "'.";

    CMACCESS2 cmAcc;
    memset(&cmAcc, 0, sizeof(cmAcc));
    cmAcc.mflCtrl |= CM_ACCESS_NOUSERLIMIT;
    cmAcc.musBoxMask = 3;
    char* end = NULL;
    cmAcc.mulSerialNumber = strtoul(dongle_id.c_str(), &end, 10);

    HCMSysEntry hcmse2 = CmAccess2(CM_ACCESS_LOCAL, &cmAcc);
    if(hcmse2)
    {
        CMLTREQUEST cmLtRequest;
        memset(&cmLtRequest, 0, sizeof(cmLtRequest));
        cmLtRequest.mulFirmCode = 6000274;
        char* pRequest = new char[8192];
        memset(pRequest, 0, 8192);
        int res = CmLtCreateContext(hcmse2, CM_LT_PUSH, &cmLtRequest, NULL, 0, pRequest, 8192);
        if(res){
            context = pRequest;
            LOG(DEBUG)<< "Context successfully got.";
        }else{
            LOG(ERROR)<< "CmLtCreateContext failed, ???";
        }
        delete[] pRequest;

        if(!CmRelease(hcmse2)){
            LOG(ERROR)<< "CmRelease failed.";
        }
    }else{
        LOG(ERROR)<< "Getting context for dongle '" << dongle_id << "' failed, dongle not found.";
    }
}

void LicenseManager::Update(const string& dongle_id, const string& rau_data){
    LOG(DEBUG)<< "Updating dongle '" << dongle_id << "'.";

    CMACCESS2 cmAcc;
    memset(&cmAcc, 0, sizeof(cmAcc));
    cmAcc.mflCtrl |= CM_ACCESS_NOUSERLIMIT;
    cmAcc.musBoxMask = 3;
    char* end = NULL;
    cmAcc.mulSerialNumber = strtoul(dongle_id.c_str(), &end, 10);

    HCMSysEntry hcmse2 = CmAccess2(CM_ACCESS_LOCAL, &cmAcc);
    if(hcmse2){
        int res = CmLtImportUpdate(hcmse2, CM_LT_PUSH, rau_data.c_str(), rau_data.length());
        if(0 != res)        {
            LOG(DEBUG)<< "Dongle successfully updated.";
        }else{
            LOG(ERROR)<< "Updating dongle '" << dongle_id << "' failed, ???.";
        }

        if(!CmRelease(hcmse2)){
            LOG(WARNING)<< "CmRelease failed.";
        }
    }else{
       /* Kein passender Eintrag oder CmContainer gefunden. */
        LOG(ERROR)<< "Updating dongle '" << dongle_id << "' failed, dongle not found.";
    }
}

size_t LicenseManager::GetLicenseCount(const string& dongle_id, const string& product_id){
    LOG(DEBUG)<< "Getting license count for dongle '" << dongle_id << "' and product id '" << product_id << "'.";
    LOG(DEBUG)<< "License count successfully got.";
    return 0;
}


