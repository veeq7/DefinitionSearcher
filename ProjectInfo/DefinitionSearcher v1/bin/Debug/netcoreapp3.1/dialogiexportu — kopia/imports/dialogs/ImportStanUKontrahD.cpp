#include "StdAfx.h"
#include "managerdll.h"
#include "ImportStanukontrahd.h"

#include "docinfo.h"
#include "wti.h"
//DIALOGIEXPORTU
#include "../excel/excelstanukontrahimport.h"
#include "../CommonStanUKontrahImport.h"
#include "../csv/CSVStanUKontrahImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "apps/aura/AuraStanyUKontrahImport.h"
//Szczegolowe

// CImportStanUKontrahD dialog

IMPLEMENT_DYNAMIC(CImportStanUKontrahD, CMyDialog)
CImportStanUKontrahD::CImportStanUKontrahD()
    : CMyDialog(MYIDD(IDD_STANYUKONTRAH_IMPORT), NULL)
    , m_k_idklienta(DI_PZ, false)
{
    setRegSaveDataT(_T("Import\\StanUKontrah"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_i_cfiexcel.addRadio(&m_i_aura, _T("AURA"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_StanyuKl, true);
}

CImportStanUKontrahD::~CImportStanUKontrahD()
{
    saveRegSaveDataT();
}

void CImportStanUKontrahD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_K_IDKLIENTA, m_k_idklienta);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_I_AURA, m_i_aura);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportStanUKontrahD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("k_idklienta", m_k_idklienta);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportStanUKontrahD::UkryjCoNiepotrzebne()
{
    m_fileName.setFilter(getFileMask());

    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}

BEGIN_MESSAGE_MAP(CImportStanUKontrahD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportStanUKontrahD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportStanUKontrahD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportStanUKontrahD_DM);
};

DIALOGMANAGER_REGISTERT("ImportStanyKontrahD", CImportStanUKontrahD_DM);

bool CImportStanUKontrahD_DM::executeForParam(const CMyString& param)
{
    CImportStanUKontrahD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportStanUKontrahD::getFileMask()
{
    CMyString f = DTGDTGETPOLET("_format");

    if (EQSTRINGT(f, "CFIEXCEL"))
        return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki XML (*.xml)|*.xml|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");

    if (EQSTRINGT(f, "AURA"))
        return _T("Pliki XML (*.xml)|*.xml|Pliki MS-Excel (*.xls)|*.xls|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");

    if (EQSTRINGT(f, "CFICSV"))
        return _T("Pliki CSV (*.csv)|*.csv|Pliki MS-Excel (*.xls)|*.xls|Pliki XML (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*||");

    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki CSV (*.csv)|*.csv|Pliki XML (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*||");
}

void CImportStanUKontrahD::OnOK()
{
    _ileZaimportowano = 0;
    _ilePominieto = 0;

    if (!isOkID(m_k_idklienta.getValue()))
    {
        AfxMessageBox(_TX("Nie wybrano kontrahenta!"));
        return;
    }

    if (m_i_cfiexcel.getValue() == _T("AURA"))
        return onAuraImport();

    CCommonStanUKontrahImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelStanUKontahImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVStanUKontrahImport();
        if (ci) ((CCSVStanUKontrahImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setIDKlienta(m_k_idklienta.getValue());
    ci->setFileName(DTGDTGETPOLET("_fileName"));
    ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        DELETE_WSK(ci);
        return;
    }

    DELETE_WSK(ci);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" pozycji (pominięto ") + CMyString(_ilePominieto) + _T(")"));

    CMyDialog::OnOK();
}

void CImportStanUKontrahD::onAuraImport()
{
    CAuraStanyUKontrahImport* auraSUKI = new CAuraStanyUKontrahImport();
    if (!auraSUKI)
        return;

    auraSUKI->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));
    auraSUKI->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    auraSUKI->setKlient(m_k_idklienta.getValue());

    CCommonWTIDlgTPL<CAuraStanyUKontrahImport>* dlg(this);
    WTI_NDLG(*dlg, auraSUKI, 1);

    if (auraSUKI->isError())
    {
        AfxMultiMessageBox(auraSUKI->getErrorMessage());
        delete auraSUKI;
        return;
    }

    delete auraSUKI;

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" pozycji (pominięto ") + CMyString(_ilePominieto) + _T(")"));

    CMyDialog::OnOK();
}

void CImportStanUKontrahD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CAuraStanyUKontrahImport>& prm)
{
    if (prm.getType() == 1)
    {
        CAuraStanyUKontrahImport* auraSUKI = prm.getParam();
        if (auraSUKI)
        {
            auraSUKI->setStatusRetHandler(sr);
            CDataT::toLog(_FX(_TX("Rozpoczęto import %s stanów u kontrahenta z pliku %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
            CCommonImportTIDImportParams prms;
            auraSUKI->importFile(DTGDTGETPOLET("_fileName"), prms);
            _ileZaimportowano = auraSUKI->ileZaimportowanych();
            _ilePominieto = auraSUKI->ilePominietych();
            CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
            auraSUKI->clearStatusRetHandler();
        }
    }
}

void CImportStanUKontrahD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonStanUKontrahImport>& prm)
{
    CCommonStanUKontrahImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s stanów u kontrahenta z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute();
        _ilePominieto += eti->ilePominieto();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s, pominęto %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano), CMyString(_ilePominieto)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


CUInfo CImportStanUKontrahD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportStanUKontrahD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_STANUKONTRAH));
}

INT_PTR CImportStanUKontrahD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_STANUKONTRAH))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
