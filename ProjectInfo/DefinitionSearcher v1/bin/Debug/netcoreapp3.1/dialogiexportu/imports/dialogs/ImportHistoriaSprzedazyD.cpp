#include "StdAfx.h"
#include "managerdll.h"
#include "ImportHistoriaSprzedazyD.h"

#include "wti.h"
#include "../excel/ExcelHistoriaSprzedazyImport.h"
//Szczegolowe


// CImportHistoriaSprzedazyD dialog

IMPLEMENT_DYNAMIC(CImportHistoriaSprzedazyD, CMyDialog)
CImportHistoriaSprzedazyD::CImportHistoriaSprzedazyD()
: CMyDialog(CImportHistoriaSprzedazyD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\HistoriaSprzedazy"));

    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_kod_klienta"), CMyString(IHKK_KOD));
    setDefVal(getDataT(), _T("_kod_towaru"), CMyString(STOWAR_BY_KOD));


    //pod kod klienta
    m_rk_kod.addRadio(&m_rk_kod, CMyString(IHKK_KOD));
    m_rk_kod.addRadio(&m_rk_nip, CMyString(IHKK_NIP));
    m_rk_kod.addRadio(&m_rk_regon, CMyString(IHKK_REGON));
    m_rk_kod.addRadio(&m_rk_iln, CMyString(IHKK_ILN));

    //pod kod towaru
    m_rt_kod.addRadio(&m_rt_kod, CMyString(STOWAR_BY_KOD));
    m_rt_kod.addRadio(&m_rt_koddost, CMyString(STOWAR_BY_KODDOST));
    m_rt_kod.addRadio(&m_rt_ean, CMyString(STOWAR_BY_EAN13));
    m_rt_kod.addRadio(&m_rt_n1, CMyString(STOWAR_BY_N1));
    m_rt_kod.addRadio(&m_rt_n2, CMyString(STOWAR_BY_N2));
    m_rt_kod.addRadio(&m_rt_n3, CMyString(STOWAR_BY_N3));
    m_rt_kod.addRadio(&m_rt_n4, CMyString(STOWAR_BY_N4));
    m_rt_kod.addRadio(&m_rt_n5, CMyString(STOWAR_BY_N5));
    m_rt_kod.addRadio(&m_rt_n6, CMyString(STOWAR_BY_N6));
}

CImportHistoriaSprzedazyD::~CImportHistoriaSprzedazyD()
{
    saveRegSaveDataT();
}

void CImportHistoriaSprzedazyD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    //pod kod klienta
    DDX_Control(pDX, IDC_RK_KOD, m_rk_kod);
    DDX_Control(pDX, IDC_RK_NIP, m_rk_nip);
    DDX_Control(pDX, IDC_RK_REGON, m_rk_regon);
    DDX_Control(pDX, IDC_RK_ILN, m_rk_iln);
    //pod kod towaru
    DDX_Control(pDX, IDC_RT_KOD, m_rt_kod);
    DDX_Control(pDX, IDC_RT_KODDOST, m_rt_koddost);
    DDX_Control(pDX, IDC_RT_EAN, m_rt_ean);
    DDX_Control(pDX, IDC_RT_N1, m_rt_n1);
    DDX_Control(pDX, IDC_RT_N2, m_rt_n2);
    DDX_Control(pDX, IDC_RT_N3, m_rt_n3);
    DDX_Control(pDX, IDC_RT_N4, m_rt_n4);
    DDX_Control(pDX, IDC_RT_N5, m_rt_n5);
    DDX_Control(pDX, IDC_RT_N6, m_rt_n6);
}

bool CImportHistoriaSprzedazyD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_kod_klienta", m_rk_kod);
    DC_REGISTERT("_kod_towaru", m_rt_kod);
    return __super::RegisterOnInit();
}

void CImportHistoriaSprzedazyD::UkryjCoNiepotrzebne()
{

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportHistoriaSprzedazyD, CMyDialog)
END_MESSAGE_MAP()



class CImportHistoriaSprzedazyD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportHistoriaSprzedazyD_DM);
};


DIALOGMANAGER_REGISTERT("ImportHistoriaSprzedazyD", CImportHistoriaSprzedazyD_DM);


bool CImportHistoriaSprzedazyD_DM::executeForParam(const CMyString& param)
{
    CImportHistoriaSprzedazyD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportHistoriaSprzedazyD::getFileMask()
{
    return _T("Pliki MS-Excel (*.xls)|*.xls|Wszystkie pliki (*.*)|*.*||");
}


void CImportHistoriaSprzedazyD::OnOK()
{
    _ileZaimportowano = 0;

    CExcelHistoriaSprzedazyImport eki;
    eki.setHowActualize(getHowActualize());

    WTI_NDLG(*this, &eki, 0);

    if (eki.isError())
    {
        AfxMultiMessageBox(eki.getErrorMessage());
        return;
    }

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" pozycji historii sprzedazy"));


    CMyDialog::OnOK();
}

void CImportHistoriaSprzedazyD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CExcelHistoriaSprzedazyImport>& prm)
{
    CExcelHistoriaSprzedazyImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        eti->setParametruDlaSzukania((eIH_KOD_Klienta)_atoi(DTGDTGETPOLET("_kod_klienta")), (eTowarSearchBy)_atoi(DTGDTGETPOLET("_kod_towaru")));
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        eti->clearStatusRetHandler();
    }
}


int CImportHistoriaSprzedazyD::getHowActualize()
{
    return ACTUALIZE_SKIP;
}
// CImportHistoriaSprzedazyD message handlers

CUInfo CImportHistoriaSprzedazyD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportHistoriaSprzedazyD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_HISTORIA_SPRZEDAZY));
}

INT_PTR CImportHistoriaSprzedazyD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_HISTORIA_SPRZEDAZY))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
