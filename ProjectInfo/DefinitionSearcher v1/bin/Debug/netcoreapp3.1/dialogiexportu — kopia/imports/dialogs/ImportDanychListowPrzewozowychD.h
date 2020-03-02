#pragma once
// #include "../../common.h"
// 
#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"

class CCommonDaneListuPrzewozowegoImport;

class CImportDanychListowPrzewozowychD : public CMyDialog, public CCommonWTIDlgTPL<CCommonDaneListuPrzewozowegoImport>
{
    DECLARE_DYNAMIC(CImportDanychListowPrzewozowychD)
    DECLARE_MESSAGE_MAP()

public:
    CImportDanychListowPrzewozowychD();   // standard constructor
    virtual ~CImportDanychListowPrzewozowychD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    // CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonDaneListuPrzewozowegoImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;
    CMyComboCtrl m_map_kod;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    virtual void OnOK();

private:
    int _ileZaimportowano, _ilePominieto;
};
