#include "StdAfx.h"
#include "managerdll.h"
#include "ImportSrodkiTrwaleD.h"

#include "wti.h"
#include "../excel/ExcelSrodkiTrwaleImport.h"
#include "../CommonSrodkiTrwaleImport.h"
#include "../csv/CSVSrodkiTrwaleImport.h"
#include "dialogs/male/CSVParametryD.h"
//Szczegolowe


// CImportSrodkiTrwaleD dialog

IMPLEMENT_DYNAMIC(CImportSrodkiTrwaleD, CMyDialog)
CImportSrodkiTrwaleD::CImportSrodkiTrwaleD()
: CMyDialog(CImportSrodkiTrwaleD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\SrodkiTrwale"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_SrodkiTrw, true);
}

CImportSrodkiTrwaleD::~CImportSrodkiTrwaleD()
{
    saveRegSaveDataT();
}

void CImportSrodkiTrwaleD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportSrodkiTrwaleD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportSrodkiTrwaleD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportSrodkiTrwaleD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportSrodkiTrwaleD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}



class CImportSrodkiTrwaleD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportSrodkiTrwaleD_DM);
};


DIALOGMANAGER_REGISTERT("ImportSrodkiTrwaleD", CImportSrodkiTrwaleD_DM);


bool CImportSrodkiTrwaleD_DM::executeForParam(const CMyString& param)
{
    CImportSrodkiTrwaleD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportSrodkiTrwaleD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}


void CImportSrodkiTrwaleD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonSrodkiTrwaleImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelSrodkiTrwaleImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVSrodkiTrwaleImport();
        if (ci) ((CCSVSrodkiTrwaleImport*)ci)->setCSVConfig(csvconf);
    }

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

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" środków trwałych"));

    CMyDialog::OnOK();
}

void CImportSrodkiTrwaleD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonSrodkiTrwaleImport>& prm)
{

    CCommonSrodkiTrwaleImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s środków trwałych z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportSrodkiTrwaleD::getHowActualize()
{
    return ACTUALIZE_SKIP;
}
// CImportSrodkiTrwaleD message handlers

CUInfo CImportSrodkiTrwaleD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportSrodkiTrwaleD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_SRODKITRWALE));
}

INT_PTR CImportSrodkiTrwaleD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_SRODKITRWALE))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
