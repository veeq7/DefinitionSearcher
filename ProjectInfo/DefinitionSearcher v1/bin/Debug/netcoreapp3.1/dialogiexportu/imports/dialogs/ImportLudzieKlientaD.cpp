#include "StdAfx.h"
#include "managerdll.h"
#include "ImportLudzieKlientaD.h"

//dialogiexportu
#include "dialogs/male/CSVParametryD.h"
#include "../interfacesgui/modules/import/ImportInFiltr.h"
#include "imports/dialogs/ImportWartosciDowD.h"
#include "../excel/ExcelLudzieKlientaImport.h"
#include "../csv/CSVLudzieKlientaImport.h"
#include "wti.h"

// CImportLudzieKlientaD dialog

IMPLEMENT_DYNAMIC(CImportLudzieKlientaD, CMyDialog)
CImportLudzieKlientaD::CImportLudzieKlientaD()
: CMyDialog(CImportLudzieKlientaD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\LudzieKlienta"));

    m_map_kod.addKeysValues(GetSearchMap(TB_KLIENT));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_mapKod"), CMyString(SKLIENT_BY_KOD));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_LudzieKlienta, true);
}

CImportLudzieKlientaD::~CImportLudzieKlientaD()
{
    saveRegSaveDataT();
}

void CImportLudzieKlientaD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_MAP_KOD, m_map_kod);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDOK, m_ok);
    DDX_Control(pDX, IDCANCEL, m_cancel);
    DDXCTRL(IDC_C_OSTRANS, m_im_osTrans);
    DDXCTRL(IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportLudzieKlientaD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_mapKod", m_map_kod);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportLudzieKlientaD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportLudzieKlientaD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportLudzieKlientaD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

CMyString CImportLudzieKlientaD::getFileMask()
{
    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki Insert TS4 (*.ts4)|*.ts4|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");
}


void CImportLudzieKlientaD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonLudzieKlientaImport* ci = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelLudzieKlientaImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVLudzieKlientaImport();
        if (ci) ((CCSVLudzieKlientaImport*)ci)->setCSVConfig(csvconf);
    }

    if (!ci) return;
    ci->setHowActualize(getHowActualize());
    ci->setMapowanieKodu((eKlientSearchBy)_atoi(DTGDTGETPOLET("_mapKod")));
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

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" osoby"));

    CMyDialog::OnOK();
}

void CImportLudzieKlientaD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonLudzieKlientaImport>& prm)
{
    CCommonLudzieKlientaImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s osób z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s osób."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportLudzieKlientaD::getHowActualize()
{
    return ACTUALIZE_UPDATE;
}

INT_PTR CImportLudzieKlientaD::DoModal()
{

    if (!CanDoIt(UPR_IMPORT_LUDZIE_KLIENTA))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }

    return __super::DoModal();
}

CUInfo CImportLudzieKlientaD::getPermission(int no)
{

    switch (no)
    {
    case 0:
        return getMainPermission();
    }

    return __super::getPermission(no - 1);
}

CUInfo CImportLudzieKlientaD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_LUDZIE_KLIENTA));
}

/////////////////////////////////////////////////////////////////
//////
/////////////////////////////////////////////////////////////////
class CImportLudzieKlientaD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportLudzieKlientaD_DM);
};


DIALOGMANAGER_REGISTERT("ImportLudzieKlientaD", CImportLudzieKlientaD_DM);


bool CImportLudzieKlientaD_DM::executeForParam(const CMyString& param)
{
    CImportLudzieKlientaD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}
