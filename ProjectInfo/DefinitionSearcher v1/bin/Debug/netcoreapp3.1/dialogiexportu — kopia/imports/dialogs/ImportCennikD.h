#pragma once
#include "../../common.h"

#include "wtih.h"
//dialogiexportu
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../kontrolki/selects/selectgrupacen.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonCennikImport;

class CImportCennikD : public CMyDialog, public CCommonWTIDlgTPL<CCommonCennikImport>
{
    DECLARE_DYNAMIC(CImportCennikD)

public:
    CImportCennikD();   // standard constructor
    virtual ~CImportCennikD();

    // Dialog Data
    enum { IDD = IDD_CENNIK_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();
    afx_msg void OnBnCsv();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonCennikImport>& prm);

    INT_PTR DoModal();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

    CMyFileEditCtrl m_fileName;
    CMyCheckBox m_c_domyslna;
    CMyCheckBox m_c_pomin;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;
    CSelectGrupaCen m_tgc_idgrupy;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_insertts4;
    CMyRadioButton m_i_cficsv;
    CMyComboCtrl m_map_kod;
    CButtonST m_csv_params;
    CButtonST m_ok;
    CButtonST m_cancel;

    CCommonCSVConfigC csvconf;

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
