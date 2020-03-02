#pragma once
#include "../../common.h"

#include "wtih.h"
//Szczegolowe
#include "../guidll/gui/adds/myfileeditctrl.h"
//Resources
#include "../../resource.h"

class CExcelHistoriaSprzedazyImport;

class CImportHistoriaSprzedazyD : public CMyDialog, public CCommonWTIDlgTPL<CExcelHistoriaSprzedazyImport>
{
    DECLARE_DYNAMIC(CImportHistoriaSprzedazyD)

public:
    CImportHistoriaSprzedazyD();   // standard constructor
    virtual ~CImportHistoriaSprzedazyD();

    // Dialog Data
    enum { IDD = IDD_HISTORIA_SPRZEDAZY_IMPORT };

    bool RegisterOnInit();
    void UkryjCoNiepotrzebne();

    int getHowActualize();
    CMyString getFileMask();

    CUInfo getMainPermission();
    CUInfo getPermission(int no);

    void executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CExcelHistoriaSprzedazyImport>& prm);
    INT_PTR DoModal();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    CMyFileEditCtrl m_fileName;
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()

    //pod szukanie klienta
    CMyRadioButton m_rk_kod;
    CMyRadioButton m_rk_nip;
    CMyRadioButton m_rk_regon;
    CMyRadioButton m_rk_iln;

    //pod szukanie towaru
    CMyRadioButton m_rt_kod;
    CMyRadioButton m_rt_koddost;
    CMyRadioButton m_rt_ean;
    CMyRadioButton m_rt_n1;
    CMyRadioButton m_rt_n2;
    CMyRadioButton m_rt_n3;
    CMyRadioButton m_rt_n4;
    CMyRadioButton m_rt_n5;
    CMyRadioButton m_rt_n6;

private:
    int _ileZaimportowano;
};
