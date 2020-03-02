#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../kontrolki/selects/selectmagazyn.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonNormatywyImport;

class CImportNormatywowD : public CMyDialog, public CCommonWTIDlgTPL<CCommonNormatywyImport>
{
    DECLARE_DYNAMIC(CImportNormatywowD)

public:
    CImportNormatywowD();   // standard constructor
    virtual ~CImportNormatywowD();

    // Dialog Data
    enum { IDD = IDD_NORMATYWOW_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonNormatywyImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CSelectMagazyn m_tmg_idmagazynu;
    CMyRadioButton m_i_cficsv;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
