#pragma once
// #include "../../common.h"
// 
#include "wtih.h"
////dialogiexportu
////Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../kontrolki/selects/selectgrupacen.h"
#include "../common/CommonImportDataProvider.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"


class CImportDataProviderC;

class CImportKatalogPrzynaleznosciD : public CMyDialog, public CCommonWTIDlgTPL<CImportDataProviderC>
{
    DECLARE_DYNAMIC(CImportKatalogPrzynaleznosciD)

public:
    CImportKatalogPrzynaleznosciD();   // standard constructor
    virtual ~CImportKatalogPrzynaleznosciD();

    // Dialog Data
    enum { IDD = IDD_IMPORT_KATALOG_PRZYNALEZNOSCI };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();
    afx_msg void OnBnCsv();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CImportDataProviderC>& prm);

    INT_PTR DoModal();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
    CCommonImportDataProvider* getDataProvider();

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cfixml;
    CMyRadioButton m_i_cficsv;
    CButtonST m_csv_params;
    CMyRadioButton m_noex_pomin;
    CMyRadioButton m_noex_utworz;
    CMyRadioButton m_noex_blad;
    CMyComboCtrl m_map_kod;
    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;
    CButtonST m_ok;
    CButtonST m_cancel;

    CCommonCSVConfigC csvconf;

    DECLARE_MESSAGE_MAP()
private:
    int _ileZaimportowano;
};
