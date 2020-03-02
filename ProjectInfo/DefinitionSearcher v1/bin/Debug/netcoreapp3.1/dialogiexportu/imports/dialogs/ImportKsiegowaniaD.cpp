#include "StdAfx.h"
#include "managerdll.h"
#include "ImportKsiegowaniaD.h"
//dialogiexportu
#include "imports/CommonImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
#include "../../hs/const.h"
//szczegolowe
#include "../doclogic/wzorce/KsiegowanieWzorcemCSV.h"
#include "../doclogic/wzorce/KsiegowanieWzorcemXLS.h"
#include "../doclogic/wzorce/dest/KsiegowanieWzorcemOutDatabase.h"

#include "../../resource.h"

// CImportKsiegowaniaD dialog

IMPLEMENT_DYNAMIC(CImportKsiegowaniaD, CMyDialog)
CImportKsiegowaniaD::CImportKsiegowaniaD(const CMyString& zk_idzapisu)
: CMyDialog(CImportKsiegowaniaD::IDD, NULL)
, zk_idzapisu(zk_idzapisu)
{
    setRegSaveDataT(_T("Import\\Ksiegowanie"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));
    getDataT()->setPole(_T("zk_idzapisu"), zk_idzapisu);

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Ksiegowania, true);
}

CImportKsiegowaniaD::~CImportKsiegowaniaD()
{
    saveRegSaveDataT();
}

void CImportKsiegowaniaD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDOK, m_ok);
    DDX_Control(pDX, IDCANCEL, m_cancel);
}

bool CImportKsiegowaniaD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    return __super::RegisterOnInit();
}

void CImportKsiegowaniaD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportKsiegowaniaD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportKsiegowaniaD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

CMyString CImportKsiegowaniaD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}


void CImportKsiegowaniaD::OnOK()
{
    _ileZaimportowano = 0;

    CDialogManagerItemExC it = DM_FINDITEMT("WyborWzorcaSmallD");
    if (!it) return;

    CMyHashMapC prms;
    prms.SetAt(_T("typRokuKH"), CMyString(CKHZapisyHeadT::getTypRokuKHS(zk_idzapisu)));

    CMyString ret = it.executeWithResponse(KH::WZ_TYP_FROMXLS, prms);
    if (!isOkID(ret)) return;

    CKsiegowanieWzorcemBase* kwc = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL"))
    {
        kwc = new CKsiegowanieWzorcemXLS(ret);
        ((CKsiegowanieWzorcemXLS*)kwc)->setFileName(GET_POLE(_T("_fileName")));
    }
    else if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        kwc = new CKsiegowanieWzorcemCSV(ret);
        ((CKsiegowanieWzorcemCSV*)kwc)->setFileName(GET_POLE(_T("_fileName")));
        ((CKsiegowanieWzorcemCSV*)kwc)->setCSVConfig(csvconf);
    }

    if (kwc)
    {
        //Ustaw outDestination-----------------------------------------------------------------------------------------------------------------
        KSIEGOWANIEWZORCEM::CKsiegowanieWzorcemOutDatabase* kwdb = new KSIEGOWANIEWZORCEM::CKsiegowanieWzorcemOutDatabase(YKH_KH, GET_POLE(_T("zk_idzapisu")));
        kwc->setOutDestination(kwdb);

        WTI_NDLG(*this, kwc, 0);

        if (kwc->isError())
        {
            AfxMultiMessageBox(kwc->getErrorMessage());
            delete kwc;
            return;
        }

        delete kwc;
    }

    CMyDialog::OnOK();
}

void CImportKsiegowaniaD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CKsiegowanieWzorcemBase>& prm)
{
    CKsiegowanieWzorcemBase* kwc = prm.getParam();
    if (kwc)
    {
        kwc->setStatusRetHandler(sr);
        kwc->ProcessIt();
        kwc->clearStatusRetHandler();
    }
}


int CImportKsiegowaniaD::getHowActualize()
{
    return ACTUALIZE_SKIP;
}


/////////////////////////////////////////////////////////////////
//////
/////////////////////////////////////////////////////////////////
class CImportKsiegowaniaD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportKsiegowaniaD_DM);
};

DIALOGMANAGER_REGISTERT("ImportKsiegowaniaD", CImportKsiegowaniaD_DM);


bool CImportKsiegowaniaD_DM::executeForParam(const CMyString& param)
{
    CImportKsiegowaniaD dlg(param);
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}
