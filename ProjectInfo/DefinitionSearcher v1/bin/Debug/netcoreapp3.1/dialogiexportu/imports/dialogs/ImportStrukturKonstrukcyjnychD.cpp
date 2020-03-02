#include "StdAfx.h"
#include "managerdll.h"
#include "ImportStrukturKonstrukcyjnychD.h"

#include "../excel/ExcelStrukturKonstrukcyjnychImport.h"
#include "../CommonStrukturKonstrukcyjnychImport.h"
#include "../csv/CSVStrukturKonstrukcyjnychImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
#include "apps/CommonImportTID.h"
#include "apps/procad/ProcadStrukturyImport.h"
//Szczegolowe

// CImportStrukturKonstrukcyjnychD dialog

IMPLEMENT_DYNAMIC(CImportStrukturKonstrukcyjnychD, CMyDialog)
CImportStrukturKonstrukcyjnychD::CImportStrukturKonstrukcyjnychD()
    : CMyDialog(MYIDD(IDD_STRUKTURAKON_IMPORT), NULL)
{
    setRegSaveDataT(_T("Import\\StrukturyKonst"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_i_cfiexcel.addRadio(&m_i_procadxml, _T("PROCADXML"));

    m_fileName.setFilter(getFileMask());

    m_radio_aktualizacja.addRadio(&m_radio_aktualizacja, CMyString(ISK_STISNIEJE_AKTUALIZACJA));
    m_radio_aktualizacja.addRadio(&m_radio_wersja, CMyString(ISK_STISNIEJE_WERSJA));
    m_radio_aktualizacja.addRadio(&m_radio_relacja, CMyString(ISK_STISNIEJE_RELACJA));

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_r_istnieje"), CMyString(ISK_STISNIEJE_AKTUALIZACJA));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_StrukturyKonstrukcyjne, true);
}

CImportStrukturKonstrukcyjnychD::~CImportStrukturKonstrukcyjnychD()
{
    saveRegSaveDataT();
}

BEGIN_MESSAGE_MAP(CImportStrukturKonstrukcyjnychD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportStrukturKonstrukcyjnychD::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDXExchange(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXExchange(pDX, IDC_I_PROCADXML, m_i_procadxml);
    DDXExchange(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDXExchange(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDXExchange(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDXExchange(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
    DDXExchange(pDX, IDC_C_ZALOZ_KT, m_zaloz_kt);
    DDXExchange(pDX, IDC_C_ZATWIERDZENIE, m_autozatwierdzanie);
    DDXExchange(pDX, IDC_C_DWUETAPOWY, m_c_dwuetapowy);
    DDXExchange(pDX, IDC_R_AKT, m_radio_aktualizacja);
    DDXExchange(pDX, IDC_R_WERSJA, m_radio_wersja);
    DDXExchange(pDX, IDC_R_RELACJA, m_radio_relacja);
}

bool CImportStrukturKonstrukcyjnychD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    DC_REGISTERT("_zaloz_kt", m_zaloz_kt);
    DC_REGISTERT("_auto_zatwierdzenie", m_autozatwierdzanie);
    DC_REGISTERT("_c_dwuetapowy", m_c_dwuetapowy);
    DC_REGISTERT("_r_istnieje", m_radio_aktualizacja);

    return __super::RegisterOnInit();
}

void CImportStrukturKonstrukcyjnychD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}

void CImportStrukturKonstrukcyjnychD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

CMyString CImportStrukturKonstrukcyjnychD::getFileMask()
{
    CMyString f = DTGDTGETPOLET("_format");

    if (EQSTRINGT(f, "CFIEXCEL"))
        return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki MS-Excel 2007 (*.xlsx)|*.xlsx|Pliki CSV (*.csv)|*.csv|Pliki XML (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*||");

    if (EQSTRINGT(f, "PROCADXML"))
        return _T("Pliki XML (*.xml)|*.xml|Pliki MS-Excel (*.xls)|*.xls|Pliki MS-Excel 2007 (*.xlsx)|*.xlsx|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");

    if (EQSTRINGT(f, "CFICSV"))
        return _T("Pliki CSV (*.csv)|*.csv|Pliki MS-Excel (*.xls)|*.xls|Pliki MS-Excel 2007 (*.xlsx)|*.xlsx|Pliki XML (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*||");

    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki MS-Excel 2007 (*.xlsx)|*.xlsx|Pliki CSV (*.csv)|*.csv|Pliki XML (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*||");
}

void CImportStrukturKonstrukcyjnychD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonStrukturKonstrukcyjnychImport* ci = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL"))
        ci = new CExcelStrukturKonstrukcyjnychImport();

    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVStrukturKonstrukcyjnychImport();
        if (ci) ((CCSVStrukturKonstrukcyjnychImport*)ci)->setCSVConfig(csvconf);
    }

    if (ci)
    {
        CImportKeysCacheC keysCache(new CImportKeysCache());
        ci->setKeysCache(keysCache);
        ci->setHowActualize(getHowActualize());
        ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
        ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));
        ci->setZakladanieBrakujacychKartotek(_atoi(DTGDTGETPOLET("_zaloz_kt")));
        ci->setAutomatyczneZatwierdzanie(_atoi(DTGDTGETPOLET("_auto_zatwierdzenie")));
        ci->setDwuetapowe(_atoi(DTGDTGETPOLET("_c_dwuetapowy")));
        ci->setTypAktualizacji((eISK_StrukturaIstnieje)_atoi(DTGDTGETPOLET("_r_istnieje")));

        WTI_NDLG(*this, ci, 0);

        if (ci->isError())
        {
            AfxMultiMessageBox(ci->getErrorMessage());
            DELETE_WSK(ci);
            return;
        }

        DELETE_WSK(ci);

        AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _TX(" rekordów"));

        CMyDialog::OnOK();
    }

    CProcadStrukturyImport* procadSI = NULL;
    if (m_i_cfiexcel.getValue() == _T("PROCADXML"))
        procadSI = new CProcadStrukturyImport();

    if (procadSI)
    {
        procadSI->setHowActualize(getHowActualize());
        procadSI->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
        procadSI->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));
        procadSI->setZakladanieBrakujacychKartotek(_atoi(DTGDTGETPOLET("_zaloz_kt")));
        procadSI->setAutomatyczneZatwierdzanie(_atoi(DTGDTGETPOLET("_auto_zatwierdzenie")));
        procadSI->setTypAktualizacji((eISK_StrukturaIstnieje)_atoi(DTGDTGETPOLET("_r_istnieje")));

        CCommonWTIDlgTPL<CProcadStrukturyImport>* dlg(this);
        WTI_NDLG(*dlg, procadSI, 1);

        if (procadSI->isError())
        {
            AfxMultiMessageBox(procadSI->getErrorMessage());
            DELETE_WSK(procadSI);
            return;
        }

        DELETE_WSK(procadSI);

        AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _TX(" rekordów"));

        CMyDialog::OnOK();
    }
}

void CImportStrukturKonstrukcyjnychD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonStrukturKonstrukcyjnychImport>& prm)
{
    if (prm.getType() == 0)
    {
        CCommonStrukturKonstrukcyjnychImport* eti = prm.getParam();
        if (eti)
        {
            eti->setStatusRetHandler(sr);
            CDataT::toLog(_FX(_TX("Rozpoczęto import %s struktur produktów z pliku %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
            _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
            CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
            eti->clearStatusRetHandler();
            return;
        }
    }
}

void CImportStrukturKonstrukcyjnychD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CProcadStrukturyImport>& prm)
{
    if (prm.getType() == 1)
    {
        CProcadStrukturyImport* procadSI = prm.getParam();
        if (procadSI)
        {
            procadSI->setStatusRetHandler(sr);
            CDataT::toLog(_FX(_TX("Rozpoczęto import %s struktur produktów z pliku %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
            CCommonImportTIDImportParams prms;
            _ileZaimportowano = procadSI->importFile(DTGDTGETPOLET("_fileName"), prms);
            CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
            procadSI->clearStatusRetHandler();
            return;
        }
    }
}

int CImportStrukturKonstrukcyjnychD::getHowActualize()
{
    return ACTUALIZE_SKIP;
}
// CImportStrukturKonstrukcyjnychD message handlers

// Upr
CUInfo CImportStrukturKonstrukcyjnychD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportStrukturKonstrukcyjnychD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_CZZAMIENNE));
}

INT_PTR CImportStrukturKonstrukcyjnychD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_CZZAMIENNE))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}

//############################################################################

class CImportStrukturKonstrukcyjnychD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportStrukturKonstrukcyjnychD_DM);
};

DIALOGMANAGER_REGISTERT("ImportStrukturKonstrukcyjnychD", CImportStrukturKonstrukcyjnychD_DM);

bool CImportStrukturKonstrukcyjnychD_DM::executeForParam(const CMyString& param)
{
    CImportStrukturKonstrukcyjnychD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}
