#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../kontrolki/selects/selectmagazyn.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonStanMagazynowyImport;

class CImportStanMagazynowyD : public CMyDialog, public CCommonWTIDlgTPL<CCommonStanMagazynowyImport>
{
    DECLARE_DYNAMIC(CImportStanMagazynowyD)

public:
    CImportStanMagazynowyD();   // standard constructor
    virtual ~CImportStanMagazynowyD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    afx_msg void OnBnCsv();

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonStanMagazynowyImport>& prm);
    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;

protected:
    CSelectMagazyn m_tmg_idmagazynu;
};
