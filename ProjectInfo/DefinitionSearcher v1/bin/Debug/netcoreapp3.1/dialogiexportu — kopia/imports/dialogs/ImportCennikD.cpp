#include "StdAfx.h"
#include "managerdll.h"
#include "ImportCennikD.h"
//dialogiexportu
#include "dialogs/male/CSVParametryD.h"
#include "../interfacesgui/modules/import/ImportInFiltr.h"
#include "imports/dialogs/ImportWartosciDowD.h"
#include "../excel/ExcelCennikImport.h"
#include "../insert/insertts4cennikimport.h"
#include "../csv/CSVCennikImport.h"
#include "wti.h"

// CImportCennikD dialog

IMPLEMENT_DYNAMIC(CImportCennikD, CMyDialog)
CImportCennikD::CImportCennikD()
: CMyDialog(CImportCennikD::IDD, NULL)
, m_tgc_idgrupy(SGC__GRUPYSPRZEDAZY)
{
    setRegSaveDataT(_T("Import\\Cennik"));

    m_map_kod.addKeysValues(GetSearchMap(TG_TOWARY));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_insertts4, _T("TS4"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_mapKod"), CMyString(STOWAR_BY_KOD));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Cennik, true);
}

CImportCennikD::~CImportCennikD()
{
    saveRegSaveDataT();
}

void CImportCennikD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_INSERTTS4, m_i_insertts4);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_TGC_IDGRUPY, m_tgc_idgrupy);
    DDX_Control(pDX, IDC_C_DOMYSLNA, m_c_domyslna);
    DDX_Control(pDX, IDC_C_POMIN, m_c_pomin);
    DDX_Control(pDX, IDC_MAP_KOD, m_map_kod);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDOK, m_ok);
    DDX_Control(pDX, IDCANCEL, m_cancel);
    DDXCTRL(IDC_C_OSTRANS, m_im_osTrans);
    DDXCTRL(IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportCennikD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("c_domyslna", m_c_domyslna);
    DC_REGISTERT("c_pomin", m_c_pomin);
    DC_REGISTERT("tgc_idgrupy", m_tgc_idgrupy);
    DC_REGISTERT("_mapKod", m_map_kod);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportCennikD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportCennikD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportCennikD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

CMyString CImportCennikD::getFileMask()
{
    return _T("Pliki MS-Excel (*.xls)|*.xls|Pliki Insert TS4 (*.ts4)|*.ts4|Pliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||");
}


void CImportCennikD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonCennikImport* ci = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelCennikImport();
    if (m_i_cfiexcel.getValue() == _T("TS4")) ci = new CInsertTS4CennikImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVCennikImport();
        if (ci) ((CCSVCennikImport*)ci)->setCSVConfig(csvconf);
    }

    if (!ci) return;
    ci->setHowActualize(getHowActualize());
    ci->setGrupaCen(DTGDTGETPOLET("tgc_idgrupy"));
    ci->setJakBrakKart(_atoi(DTGDTGETPOLET("c_pomin")));
    ci->setCenyDomyslne(_atoi(DTGDTGETPOLET("c_domyslna")));
    ci->setMapowanieKodu((eTowarSearchBy)_atoi(DTGDTGETPOLET("_mapKod")));
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

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" cen"));

    CMyDialog::OnOK();
}

void CImportCennikD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonCennikImport>& prm)
{
    CCommonCennikImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s cen z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s cen."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportCennikD::getHowActualize()
{
    return ACTUALIZE_UPDATE;
}

/*
class CTestThread : public CMyThread
{
DECLARE_DYNCREATE(CTestThread)
public:
CTestThread() {}

INT_PTR Run();

bool onWantEndThread();

int no;
};

int threadNos(0);

INT_PTR CTestThread::Run()
{
signalBeginThread();

no=++threadNos;

srand (time(NULL));

while (mayRun())
{
CClassTransaction ct;
ct.BeginTransaction();

Sleep(abs(rand())%10);

ct.execQuery(_FT("UPDATE abcd SET cnt=cnt+1,n=%s WHERE d=%s",CMyString(no),CMyString(abs(rand())%3000)));

Sleep(abs(rand())%20);

ct.CloseTransaction();

Sleep(abs(rand())%10);
}


return onCommonRunEnd();
}

bool CTestThread::onWantEndThread()
{
return __super::onWantEndThread();
}

IMPLEMENT_DYNCREATE(CTestThread,CMyThread)

*/

INT_PTR CImportCennikD::DoModal()
{
    /*
    for (int i=0;i<20;i++)
    {
    CTestThread* th=DYNAMIC_DOWNCAST(CTestThread,NEWTHREADT(AfxBeginThread(RUNTIME_CLASS(CTestThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED),"TestThread"));

    CMyThreadPool::getPool()->AddThread(th);

    th->ResumeThread();
    th->InternalRelease();
    }

    */
    if (!CanDoIt(UPR_IMPORT_CENNIK))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }

    return __super::DoModal();
}

CUInfo CImportCennikD::getPermission(int no)
{

    switch (no)
    {
    case 0:
        return getMainPermission();
    }

    return __super::getPermission(no - 1);
}

CUInfo CImportCennikD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_CENNIK));
}

/////////////////////////////////////////////////////////////////
//////
/////////////////////////////////////////////////////////////////
class CImportCennikD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATEFUNC(importCennik, CImportCennikD_DM);
};


DIALOGMANAGER_REGISTERITEMT("ImportCennikD", CImportCennikD_DM, importCennik);


bool CImportCennikD_DM::executeForParam(const CMyString& param)
{
    CImportCennikD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}
