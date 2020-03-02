#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../kontrolki/selects/selectmagazyn.h"
#include "../kontrolki/selects/transakcje/selectseria.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"

class CCommonRozrachunkiImport;

class CImportRozrachunkiD : public CMyDialog, public CCommonWTIDlgTPL<CCommonRozrachunkiImport>
{
    DECLARE_DYNAMIC(CImportRozrachunkiD)

public:
    CImportRozrachunkiD();   // standard constructor
    virtual ~CImportRozrachunkiD();

    // Dialog Data
    enum { IDD = IDD_ROZRACHUNKI_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonRozrachunkiImport>& prm);

    afx_msg void OnBnCsv();

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
    CSelectSeria m_tr_seria;
    CMyEditCtrl m_tr_rok;
};
