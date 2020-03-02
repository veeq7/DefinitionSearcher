#include "StdAfx.h"
#include "managerdll.h"
#include "ImportCharakterystykiKlientaDlaTowaruD.h"

#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
#include "../excel/ExcelCharakterystykiKlientaDlaTowaruImport.h"
#include "../CommonCharakterystykiKlientaDlaTowaruImport.h"
#include "../csv/CSVCharakterystykiKlientaDlaTowaruImport.h"
//Szczegolowe


// CImportCharakterystykiKlientaDlaTowaruD dialog

IMPLEMENT_DYNAMIC(CImportCharakterystykiKlientaDlaTowaruD, CMyDialog)
CImportCharakterystykiKlientaDlaTowaruD::CImportCharakterystykiKlientaDlaTowaruD()
: CMyDialog(MYIDD(IDD_CHARKLIENTADLATOW_IMPORT), NULL)
{
    setRegSaveDataT(_T("Import\\CharakterystykiKlientaDlaTowaru"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_ex_pomin.addRadio(&m_ex_pomin, _T("SKIP"));
    m_ex_pomin.addRadio(&m_ex_actualize, _T("UPDATE"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_onexists"), _T("SKIP"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_JednostkiAlt, true);
}

CImportCharakterystykiKlientaDlaTowaruD::~CImportCharakterystykiKlientaDlaTowaruD()
{
    saveRegSaveDataT();
}

void CImportCharakterystykiKlientaDlaTowaruD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_EX_POMIN, m_ex_pomin);
    DDX_Control(pDX, IDC_EX_ACTUALIZE, m_ex_actualize);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportCharakterystykiKlientaDlaTowaruD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_onexists", m_ex_pomin);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportCharakterystykiKlientaDlaTowaruD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}

BEGIN_MESSAGE_MAP(CImportCharakterystykiKlientaDlaTowaruD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportCharakterystykiKlientaDlaTowaruD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportCharakterystykiKlientaDlaTowaruD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportCharakterystykiKlientaDlaTowaruD_DM);
};

DIALOGMANAGER_REGISTERT("ImportCharakterystykiKlientaDlaTowaruD", CImportCharakterystykiKlientaDlaTowaruD_DM);

bool CImportCharakterystykiKlientaDlaTowaruD_DM::executeForParam(const CMyString& param)
{
    CImportCharakterystykiKlientaDlaTowaruD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportCharakterystykiKlientaDlaTowaruD::getFileMask()
{
    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki MS-Excel 2007 (*.xlsx)|*.xlsx|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");
}

void CImportCharakterystykiKlientaDlaTowaruD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonCharakterystykiKlientaDlaTowaruImport* ci = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelCharakterystykiKlientaDlaTowaruImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVCharakterystykiKlientaDlaTowaruImport();
        if (ci) ((CCSVCharakterystykiKlientaDlaTowaruImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setHowActualize(getHowActualize());
    ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, ci, 0);

    int _ilePominieto = ci->ilePominieto();

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        delete ci;
        return;
    }

    delete ci;

    {
        CMyString komunikat = _FX(_TX("Zaimportowano %s charakterystyk"), CMyString(_ileZaimportowano));
        if (_ilePominieto) komunikat += _FX(_TX("\r\nPominięto %s charakterystyk"), CMyString(_ilePominieto));

        AfxMessageBox(komunikat);
    }

    CMyDialog::OnOK();
}

void CImportCharakterystykiKlientaDlaTowaruD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonCharakterystykiKlientaDlaTowaruImport>& prm)
{

    CCommonCharakterystykiKlientaDlaTowaruImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s charakterystyk z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportCharakterystykiKlientaDlaTowaruD::getHowActualize()
{
    if (DTGDTGETPOLET("_onexists") == _T("UPDATE"))
        return ACTUALIZE_UPDATE;

    return ACTUALIZE_SKIP;
}
// CImportCharakterystykiKlientaDlaTowaruD message handlers

// Upr
CUInfo CImportCharakterystykiKlientaDlaTowaruD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportCharakterystykiKlientaDlaTowaruD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_JEDNOSTKIALT));
}

INT_PTR CImportCharakterystykiKlientaDlaTowaruD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_JEDNOSTKIALT))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}