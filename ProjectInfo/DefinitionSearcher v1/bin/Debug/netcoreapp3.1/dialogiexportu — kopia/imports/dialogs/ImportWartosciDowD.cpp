#include "StdAfx.h"
#include "ImportWartosciDowD.h"
#include "../excel/ExcelWartDowolneImport.h"
#include "../CommonWartDowolneImport.h"
#include "../csv/CSVWartDowolneImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "../interfacesdll/modules/nmv/MVValueManager.h"
#include "../docinfo/main/getsearchmap.h"
#include "../interfacesgui/modules/import/ImportInFiltr.h"
#include "wti.h"
//Resources
#include "../../resource.h"


IMPLEMENT_DYNAMIC(CImportWartosciDowD, CMyDialog)
CImportWartosciDowD::CImportWartosciDowD()
    : CMyDialog(MYIDD(IDD_WARTOSCIDOW_IMPORT), NULL)
    , m_wartosc(TB_KLIENT)
    , m_currentMapBy(DT_DATAT)
{
    setRegSaveDataT(_T("Import\\WartosciDowolne"));

    m_wartosc.setAnyPodrodzaj(true);

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_rodzaj"), CMyString(TB_KLIENT));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_WartosciDow, true);
}

CImportWartosciDowD::~CImportWartosciDowD()
{
    changeMapBy(DT_DATAT);
    saveRegSaveDataT();
}

void CImportWartosciDowD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    try { DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN); }
    catch (...) {}
    DDXCTRL(IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXCTRL(IDC_I_CFICSV, m_i_cficsv);
    DDXCTRL(IDC_CSV_PARAMS, m_csv_params);

    DDXCTRL(IDC_RODZAJ, m_rodzaj);
    DDXCTRL(IDC_WARTOSC, m_wartosc);

    DDXCTRL(IDC_MAP, m_mapby);

    DDXCTRL(IDC_C_OSTRANS, m_im_osTrans);
    DDXCTRL(IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportWartosciDowD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);

    DC_REGISTERT("_rodzaj", m_rodzaj);
    DC_REGISTERT("_wartosc", m_wartosc);

    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);

    return __super::RegisterOnInit();
}

void CImportWartosciDowD::UkryjCoNiepotrzebne()
{
    eDataType typ = (eDataType)_atoi(DTGDTGETPOLET("_rodzaj"));
    m_wartosc.setTypWartosci(typ);

    changeMapBy(typ);

    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportWartosciDowD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportWartosciDowD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportWartosciDowD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportWartosciDowD_DM);
};


DIALOGMANAGER_REGISTERT("ImportWartosciDowD", CImportWartosciDowD_DM);


bool CImportWartosciDowD_DM::executeForParam(const CMyString& param)
{
    CImportWartosciDowD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportWartosciDowD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||", FILEDIALOG_PARAMS_XLS);
}


void CImportWartosciDowD::OnOK()
{
    _ileZaimportowano = 0;

    //sprawdzenie czy wybrana wartosc jest pod wybrany rodzaj
    if (!isOkID(DTGDTGETPOLET("_wartosc")))
        return;

    NMV::CMVInfoC inf = GETMVM()->getValueByID(DTGDTGETPOLET("_wartosc"));
    if (inf.isEmpty())
    {
        AfxMessageBox(_T("Nie znaleziono podanej wartości dowolnej!"));
        return;
    }

    eDataType typ = (eDataType)_atoi(DTGDTGETPOLET("_rodzaj"));
    if (inf->getDataTypeFor() != typ)
    {
        AfxMessageBox(_TX("Wybrana wartość dowolna nie w wybranego rodzaju"));
        return;
    }


    CCommonWartDowolneImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelWartDowolneImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CSVWartDowolneImport();
        if (ci) ((CSVWartDowolneImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setParametryImportu(typ, DTGDTGETPOLET("_wartosc"), _atoi(m_mapby.getValue()));
    ci->setHowActualize(getHowActualize());
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

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" wartości"));

    CMyDialog::OnOK();
}

void CImportWartosciDowD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonWartDowolneImport>& prm)
{

    CCommonWartDowolneImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s wartości dowolnych z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->setPlik(DTGDTGETPOLET("_fileName"));
        _ileZaimportowano = eti->performOperationEx();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportWartosciDowD::getHowActualize()
{
    return ACTUALIZE_SKIP;
}
// CImportWartosciDowD message handlers

INT_PTR CImportWartosciDowD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_WARTOSCIDOWOLNE))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}

CUInfo CImportWartosciDowD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportWartosciDowD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_WARTOSCIDOWOLNE));
}

bool CImportWartosciDowD::changeMapBy(const eDataType& newDataType)
{
    if (newDataType == m_currentMapBy) return true;

    getDataT()->setPole(_T("mapBy") + CMyString(m_currentMapBy), m_mapby.getValue());

    this->m_currentMapBy = newDataType;

    m_mapby.addKeysValues(GetSearchMap(newDataType), true);

    m_mapby.setValue(DTGDTGETPOLE(_T("mapBy") + CMyString(m_currentMapBy)));

    return true;
}
