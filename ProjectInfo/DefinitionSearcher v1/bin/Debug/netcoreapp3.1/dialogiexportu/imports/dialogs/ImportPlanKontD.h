#pragma once
#include "../../common.h"

//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "wtih.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonPlanKontImport;

class CImportPlanKontD : public CMyDialog, public CCommonWTIDlgTPL<CCommonPlanKontImport>
{
    DECLARE_DYNAMIC(CImportPlanKontD)

public:
    CImportPlanKontD();   // standard constructor
    virtual ~CImportPlanKontD();

    // Dialog Data
    enum { IDD = IDD_PLANKONT_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonPlanKontImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_ex_pomin;
    CMyRadioButton m_ex_actualize;
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
