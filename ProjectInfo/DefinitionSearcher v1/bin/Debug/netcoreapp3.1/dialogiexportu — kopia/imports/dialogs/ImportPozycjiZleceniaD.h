#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
#include "../sqldll/adds/CommonCSVConfig.h"
//Resources
#include "../../resource.h"
//#include "../../../guidll/gui/dialog/MyDialog.h"

class CCommonPozycjaZleceniaImport;

class CImportPozycjiZleceniaD : public CMyDialog, public CCommonWTIDlgTPL<CCommonPozycjaZleceniaImport>
{
    DECLARE_DYNAMIC(CImportPozycjiZleceniaD)

public:
    CImportPozycjiZleceniaD(const CDTTypeID& tid);   // standard constructor
    virtual ~CImportPozycjiZleceniaD();

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonPozycjaZleceniaImport>& prm);

    afx_msg void OnBnCsv();

    INT_PTR DoModal();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    CMyRadioButton m_i_cfiexcel;
    CMyRadioButton m_i_cficsv;

    CMyEditCtrl m_przelicznik_pozycji; //IDC_PRZELICZNIK_POZYCJI
    CMyCheckBox m_c_anulowanie_nieznalezionych; //IDC_C_ANULOWANIE_NIEZNALEZIONYCH
    CMyRadioButton m_r_aktualizuj; //IDC_R_AKTUALIZUJ
    CMyRadioButton m_r_pomin; //IDC_R_POMIN

    CMyCheckBox m_im_nieUwzglNowych; //IDC_C_NIEUWZGLEDNIAJ_NOWYCH

    CMyCheckBox m_im_osTrans;
    CMyCheckBox m_im_bladPrzerywa;

    CButtonST m_csv_params;
    CCommonCSVConfigC csvconf;

    virtual void OnOK();

    DECLARE_MESSAGE_MAP()

private:
    int _ileZaimportowano;
    int _ilePominieto;
    CMyString zl_idzlecenia;
    CMyString pz_idplanu;

    eDataType dataType;
};
