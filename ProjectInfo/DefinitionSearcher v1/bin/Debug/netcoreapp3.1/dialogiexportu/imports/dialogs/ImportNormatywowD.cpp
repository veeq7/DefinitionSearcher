#include "StdAfx.h"
#include "managerdll.h"
#include "ImportNormatywowD.h"

#include "wti.h"
//DIALOGIEXPORTU
#include "../excel/excelNormatywyImport.h"
#include "../CommonNormatywyImport.h"
#include "../csv/CSVNormatywyImport.h"
#include "dialogs/male/CSVParametryD.h"
//Szczegolowe


// CImportNormatywowD dialog

IMPLEMENT_DYNAMIC(CImportNormatywowD, CMyDialog)
CImportNormatywowD::CImportNormatywowD()
: CMyDialog(CImportNormatywowD::IDD, NULL), m_tmg_idmagazynu(false)
{
    setRegSaveDataT(_T("Import\\Normatywow"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_NormatywyTow, true);
}

CImportNormatywowD::~CImportNormatywowD()
{
    saveRegSaveDataT();
}

void CImportNormatywowD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_TMG_IDMAGAZYNU, m_tmg_idmagazynu);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportNormatywowD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("tmg_idmagazynu", m_tmg_idmagazynu);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportNormatywowD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportNormatywowD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportNormatywowD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}



class CImportNormatywowD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportNormatywowD_DM);
};


DIALOGMANAGER_REGISTERT("ImportNormatywowD", CImportNormatywowD_DM);


bool CImportNormatywowD_DM::executeForParam(const CMyString& param)
{
    CImportNormatywowD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportNormatywowD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}


void CImportNormatywowD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonNormatywyImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelNormatywyImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVNormatywyImport();
        if (ci) ((CCSVNormatywyImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setHowActualize(getHowActualize());
    ci->setMagazyn(DTGDTGETPOLET("tmg_idmagazynu"));
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

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" normatywów"));

    CMyDialog::OnOK();
}

void CImportNormatywowD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonNormatywyImport>& prm)
{

    CCommonNormatywyImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s normatywy towarów z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportNormatywowD::getHowActualize()
{
    return ACTUALIZE_UPDATE;
}
// CImportNormatywowD message handlers

CUInfo CImportNormatywowD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportNormatywowD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_NORMATYWOW));
}

INT_PTR CImportNormatywowD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_NORMATYWOW))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
