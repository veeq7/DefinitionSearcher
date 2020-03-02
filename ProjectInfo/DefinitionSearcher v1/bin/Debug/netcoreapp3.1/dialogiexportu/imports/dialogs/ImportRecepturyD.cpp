#include "StdAfx.h"
#include "managerdll.h"
#include "ImportRecepturyD.h"

#include "../excel/ExcelRecepturyImport.h"
#include "../CommonRecepturyImport.h"
#include "../csv/CSVRecepturyImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
//Szczegolowe


// CImportRecepturyD dialog

IMPLEMENT_DYNAMIC(CImportRecepturyD, CMyDialog)
CImportRecepturyD::CImportRecepturyD()
: CMyDialog(CImportRecepturyD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\Receptury"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Receptury, true);
}

CImportRecepturyD::~CImportRecepturyD()
{
    saveRegSaveDataT();
}

void CImportRecepturyD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportRecepturyD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportRecepturyD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportRecepturyD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportRecepturyD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportRecepturyD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportRecepturyD_DM);
};


DIALOGMANAGER_REGISTERT("ImportRecepturyD", CImportRecepturyD_DM);


bool CImportRecepturyD_DM::executeForParam(const CMyString& param)
{
    CImportRecepturyD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportRecepturyD::getFileMask()
{
    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki MS-Excel 2007 (*.xlsx)|*.xlsx|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");
}


void CImportRecepturyD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonRecepturyImport* ci = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL"))
        ci = new CExcelRecepturyImport();

    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVRecepturyImport();
        if (ci) ((CCSVRecepturyImport*)ci)->setCSVConfig(csvconf);
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

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _TX(" rekordów"));

    CMyDialog::OnOK();
}

void CImportRecepturyD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonRecepturyImport>& prm)
{
    CCommonRecepturyImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s receptur z pliku %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportRecepturyD::getHowActualize()
{
    return ACTUALIZE_SKIP;
}
// CImportRecepturyD message handlers

// Upr
CUInfo CImportRecepturyD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportRecepturyD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_RECEPTURY));
}

INT_PTR CImportRecepturyD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_RECEPTURY))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}