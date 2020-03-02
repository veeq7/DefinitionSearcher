#include "StdAfx.h"
#include "managerdll.h"
#include "ImportRozchodTechnolD.h"

//dialogiexportu
#include "dialogs/male/CSVParametryD.h"
#include "../interfacesgui/modules/import/ImportInFiltr.h"
#include "imports/dialogs/ImportWartosciDowD.h"
#include "../CommonRozchodTechnolImport.h"
#include "../excel/ExcelRozchodTechnolImport.h"
#include "../csv/CSVRozchodTechnolImport.h"
#include "wti.h"

// CImportLudzieKlientaD dialog

IMPLEMENT_DYNAMIC(CImportRozchodTechnolD, CMyDialog)
CImportRozchodTechnolD::CImportRozchodTechnolD(const CDTTypeID& main_tid, const CMyString& elem_id, const eRROZCHODU& rrozchodu/* = RROZCHOD_UNKNOWN*/)
: CMyDialog(MYIDD(IDD_ROZCHODTECHNOL_IMPORT), NULL)
{
    CMyString regSaveDataTStr = getRegSaveDataTStr(main_tid, elem_id, rrozchodu);
    setRegSaveDataT(regSaveDataTStr);

    m_map_kod.addKeysValues(GetSearchMap(TG_TOWARY));

    this->main_tid = main_tid;
    this->elem_id = elem_id;
    this->rrozchodu = rrozchodu;

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));

    m_r_rozchod.addRadio(&m_r_rozchod, ONESTRING);
    m_r_rozchod.addRadio(&m_r_przychod, ZEROSTRING);

    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    setDefVal(getDataT(), _T("_mapKod"), CMyString(STOWAR_BY_KOD));

    switch (rrozchodu)
    {
    case RROZCHOD_UNKNOWN:
        setDefVal(getDataT(), _T("_rozchod_przychod"), ONESTRING);
        break;
    case RROZCHOD_ROZCHOD:
        getDataT()->setPole(_T("_rozchod_przychod"), ONESTRING);
        break;
    case RROZCHOD_PRZYCHOD:
        getDataT()->setPole(_T("_rozchod_przychod"), ZEROSTRING);
        break;
    case RROZCHOD_ZAM_ROZCHOD:
    case RROZCHOD_NARZEDZIA:
    case RROZCHOD_KALK_MATERIALY:
    case RROZCHOD_KALK_PRZYCHODY:
    case RROZCHOD_KALK_NARZEDZIA:
    case RROZCHOD_KALK_INNY_KOSZT:
    default:
        break;
    }


    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_RozchodTechnol, true);
}

CMyString CImportRozchodTechnolD::getRegSaveDataTStr(const CDTTypeID& main_tid, const CMyString& elem_id, const eRROZCHODU& rrozchodu)
{
    CMyString ret = _T("Import\\");
    CMyString _reg = _T("RozchodTechnol");

    switch (main_tid.getType())
    {
    case TR_TECHNOLOGIE:
        _reg = isOkID(elem_id) ? _T("RozchodTechnol") : _T("RozchodTechnols");
        break;
    case TR_MRPKKWHEAD:
        _reg = isOkID(elem_id) ? _T("RozchodKKWNod") : _T("RozchodKKWNods");
        break;
    }

    ret += _reg;
    ret += _FT("\\%s", CMyString(rrozchodu));
    return ret;
}

CImportRozchodTechnolD::~CImportRozchodTechnolD()
{
    saveRegSaveDataT();
}

void CImportRozchodTechnolD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDXExchange(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDXExchange(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDXExchange(pDX, IDC_R_ROZCHOD, m_r_rozchod);
    DDXExchange(pDX, IDC_R_PRZYCHOD, m_r_przychod);
    DDXExchange(pDX, IDC_MAP_KOD, m_map_kod);
    DDXExchange(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDXExchange(pDX, IDOK, m_ok);
    DDXExchange(pDX, IDCANCEL, m_cancel);
    DDXExchange(pDX, IDC_CH_BRAK_KARTOTEKI, m_im_crete_towar);
    DDXExchange(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDXExchange(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportRozchodTechnolD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_rozchod_przychod", m_r_rozchod);
    DC_REGISTERT("_mapKod", m_map_kod);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    DC_REGISTERT("_im_crete_towar", m_im_crete_towar);
    return __super::RegisterOnInit();
}

void CImportRozchodTechnolD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    if (rrozchodu != RROZCHOD_UNKNOWN)
    {
        _DISABLE_ITO(m_r_rozchod);
        _DISABLE_ITO(m_r_przychod);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportRozchodTechnolD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportRozchodTechnolD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

CMyString CImportRozchodTechnolD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}

void CImportRozchodTechnolD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonRozchodTechnolImport* ci = NULL;

    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelRozchodTechnolImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVRozchodTechnolImport();
        if (ci) ((CCSVRozchodTechnolImport*)ci)->setCSVConfig(csvconf);
    }

    if (!ci) return;
    ci->setHowActualize(DTGDTGETPOLEINTT("_im_crete_towar"));
    ci->setMapowanieKodu((eTowarSearchBy)DTGDTGETPOLEINTT("_mapKod"));
    ci->setOsobneTransakcje(DTGDTGETPOLEINTT("_im_ostrans"));
    ci->setBladPrzerywa(DTGDTGETPOLEINTT("_im_bladprzerywa"));
    ci->setMainTID(main_tid);
    ci->setElemID(elem_id);
    ci->setRozchod(DTGDTGETPOLEINTT("_rozchod_przychod"));

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        DELETE_WSK(ci);
        return;
    }

    DELETE_WSK(ci);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" towarów"));

    CMyDialog::OnOK();
}

void CImportRozchodTechnolD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonRozchodTechnolImport>& prm)
{
    CCommonRozchodTechnolImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s towarów z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s towarów."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}

int CImportRozchodTechnolD::getHowActualize()
{
    return ACTUALIZE_UPDATE;
}

INT_PTR CImportRozchodTechnolD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_RECEPTURY))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }

    return __super::DoModal();
}

CUInfo CImportRozchodTechnolD::getPermission(int no)
{
    switch (no)
    {
    case 0: return getMainPermission();
    }

    return __super::getPermission(no - 1);
}

CUInfo CImportRozchodTechnolD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_RECEPTURY));
}

BOOL CImportRozchodTechnolD::OnInitDialog()
{

    BOOL w = CMyDialog::OnInitDialog();

    CMyString txt = _TX("Import ");

    switch (rrozchodu)
    {
    case RROZCHOD_UNKNOWN:
        txt += _TX("rozchodów/przychodów");
        break;
    case RROZCHOD_ROZCHOD:
        txt += _TX("rozchodów");
        break;
    case RROZCHOD_PRZYCHOD:
        txt += _TX("przychodów");
        break;
    case RROZCHOD_ZAM_ROZCHOD:
    case RROZCHOD_NARZEDZIA:
    case RROZCHOD_KALK_MATERIALY:
    case RROZCHOD_KALK_PRZYCHODY:
    case RROZCHOD_KALK_NARZEDZIA:
    case RROZCHOD_KALK_INNY_KOSZT:
    default:
        txt += CMyString(rrozchodu);
        break;
    }

    switch (main_tid.getType())
    {
    case TR_TECHNOLOGIE:
        txt += _TX(" technologii");
        break;
    case TR_MRPKKWHEAD:
        txt += _TX(" KKW");
        break;
    }

    setWindowText(txt);
    return w;
}

/////////////////////////////////////////////////////////////////
//////
/////////////////////////////////////////////////////////////////
class CImportRozchodTechnolD_DM : public CDialogManagerItem
{
public:
    bool executeForDMEAInner(CDMEventArgs& args);

    DIALOGMANAGER_ITEMCREATE(CImportRozchodTechnolD_DM);
};


DIALOGMANAGER_REGISTERT("ImportRozchodTechnolD", CImportRozchodTechnolD_DM);


bool CImportRozchodTechnolD_DM::executeForDMEAInner(CDMEventArgs& args)
{
    CMyHashMapC params = args.getMapParam();

    CDTTypeID main_tid = CDTTypeID();
    CMyString elem_id = EMPTYSTRING;
    eRROZCHODU rrozchodu = RROZCHOD_UNKNOWN;

    if (hasParam(_T("main_tid"), params))
        main_tid.getFromString(getParam(_T("main_tid"), params));

    if (hasParam(_T("elem_id"), params))
        elem_id = getParam(_T("elem_id"), params);

    if (hasParam(_T("rrozchodu"), params))
        rrozchodu = (eRROZCHODU)getParamInt(_T("rrozchodu"), params);

    CImportRozchodTechnolD dlg(main_tid, elem_id, rrozchodu);
    dlg.DoModalEx(args);
    return args.hasReturnValue();
}
