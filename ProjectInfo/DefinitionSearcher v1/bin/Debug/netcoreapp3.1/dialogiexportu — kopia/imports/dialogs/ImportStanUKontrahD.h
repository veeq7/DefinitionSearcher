#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../kontrolki/n/klient/selectklientn.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"


class CCommonStanUKontrahImport;
class CAuraStanyUKontrahImport;

class CImportStanUKontrahD
    : public CMyDialog
    , public CCommonWTIDlgTPL<CCommonStanUKontrahImport>
    , public CCommonWTIDlgTPL<CAuraStanyUKontrahImport>
{
    DECLARE_DYNAMIC(CImportStanUKontrahD)

public:
    CImportStanUKontrahD();   // standard constructor
    virtual ~CImportStanUKontrahD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonStanUKontrahImport>& prm);
    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CAuraStanyUKontrahImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();

protected:
    void onAuraImport();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyRadioButton m_i_aura;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano, _ilePominieto;
protected:
    CSelectKlientN m_k_idklienta;
};
