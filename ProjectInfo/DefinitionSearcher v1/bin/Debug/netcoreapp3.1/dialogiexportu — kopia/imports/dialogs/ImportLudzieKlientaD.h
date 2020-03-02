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

class CCommonLudzieKlientaImport;

class CImportLudzieKlientaD : public CMyDialog, public CCommonWTIDlgTPL<CCommonLudzieKlientaImport>
{
    DECLARE_DYNAMIC(CImportLudzieKlientaD)

public:
    CImportLudzieKlientaD();   // standard constructor
    virtual ~CImportLudzieKlientaD();

    // Dialog Data
    enum { IDD = IDD_LUDZIEKLIENTA_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();
    afx_msg void OnBnCsv();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonLudzieKlientaImport>& prm);

    INT_PTR DoModal();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

    CMyFileEditCtrl m_fileName;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;
    CMyRadioButton m_i_cfiexcel;
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
