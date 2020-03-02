#include "StdAfx.h"
#include "managerdll.h"
#include "ImportMiejscaMagazynowe.h"
#include "../excel/ExcelMiejsceMagazynoweImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
//Resources
#include "../../resource.h"

IMPLEMENT_DYNAMIC(CImportMiejscaMagazynoweD, CMyDialog)
CImportMiejscaMagazynoweD::CImportMiejscaMagazynoweD()
    : CMyDialog(MYIDD(IDD_MIEJSCAMAGAZYNOWE_IMPORT), NULL)
    , m_tmg_idmagazynu(false, PRMAG_MODE_EDIT, true)
{
    setRegSaveDataT(_T("Import\\MiejscaMagazynowe"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
}

CImportMiejscaMagazynoweD::~CImportMiejscaMagazynoweD()
{
    saveRegSaveDataT();
}

void CImportMiejscaMagazynoweD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_TMG_IDMAGAZYNU, m_tmg_idmagazynu);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportMiejscaMagazynoweD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("tmg_idmagazynu", m_tmg_idmagazynu);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportMiejscaMagazynoweD::UkryjCoNiepotrzebne()
{
    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportMiejscaMagazynoweD, CMyDialog)
END_MESSAGE_MAP()

class CImportMiejscaMagazynoweD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportMiejscaMagazynoweD_DM);
};


DIALOGMANAGER_REGISTERT("ImportMiejscaMagazynowe", CImportMiejscaMagazynoweD_DM);


bool CImportMiejscaMagazynoweD_DM::executeForParam(const CMyString& param)
{
    CImportMiejscaMagazynoweD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}

CMyString CImportMiejscaMagazynoweD::getFileMask()
{
    return _FT("%s|Wszystkie pliki (*.*)|*.*||", FILEDIALOG_PARAMS_XLS);
}

void CImportMiejscaMagazynoweD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonMiejsceMagazynoweImport* sm = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) sm = new CExcelMiejsceMagazynoweImport();

    sm->setIDMagazynu(DTGDTGETPOLET("tmg_idmagazynu"));
    sm->setFileName(DTGDTGETPOLET("_fileName"));
    sm->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    sm->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, sm, 0);

    if (sm->isError())
    {
        AfxMultiMessageBox(sm->getErrorMessage());
        DELETE_WSK(sm);
        return;
    }

    DELETE_WSK(sm);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" pozycji"));

    CMyDialog::OnOK();
}

void CImportMiejscaMagazynoweD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonMiejsceMagazynoweImport>& prm)
{
    CCommonMiejsceMagazynoweImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s miejsc magazynowych z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute();
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


CUInfo CImportMiejscaMagazynoweD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportMiejscaMagazynoweD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_MIEJSCAMAGAZYNOWE));
}

INT_PTR CImportMiejscaMagazynoweD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_MIEJSCAMAGAZYNOWE))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}
