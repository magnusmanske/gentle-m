#include "PlasmidCanvas.h"

// Popup menu handlers
    
void PlasmidCanvas::invokeORFpopup ( int item , wxPoint pt )
    {
    wxMenu *cm = new wxMenu ;
    context_last_orf = item ;

    cm->Append ( ORF_AS_NEW_ITEM , txt("p_orf_as_new_item") ) ;

    wxMenu *dm = new wxMenu ;
    cm->Append ( POPUP_DUMMY , txt("p_dna_sequence") , dm ) ;
    dm->Append ( ORF_COPY_DNA , txt("p_orf_copy_dna") ) ;
    dm->Append ( ORF_AS_NEW_DNA , txt("p_orf_as_new_dna") ) ;
    dm->Append ( ORF_BLAST_DNA , txt("m_blast_dna") ) ;
    
    wxMenu *am = new wxMenu ;
    cm->Append ( POPUP_DUMMY , txt("p_aa_sequence") , am ) ;
    am->Append ( ORF_COPY_AA , txt("p_orf_copy_aa") ) ;
    am->Append ( ORF_AS_NEW_AA , txt("p_orf_as_new_aa") ) ;
    am->Append ( ORF_BLAST_AA , txt("m_blast_aa") ) ;
    
    PopupMenu ( cm , pt ) ;
    delete cm ;
    }
    
wxMenu *PlasmidCanvas::invokeItemPopup ( int item , wxPoint pt , bool doreturn )
    {
    wxMenu *cm = new wxMenu ;
    context_last_item = item ;
    int rf = p->vec->items[context_last_item].getRF() ;
    
    cm->Append ( PC_ITEM_EDIT , txt("p_item_edit") ) ;
    if ( p->vec->items[item].isVisible() ) 
        cm->Append ( PC_ITEM_HIDE , txt("d_item_hide") ) ;
    else
        cm->Append ( PC_ITEM_SHOW , txt("d_item_show") ) ;
    cm->Append ( PC_ITEM_DELETE , txt("d_del_item") ) ;
    cm->AppendSeparator () ;

    wxMenu *dm = new wxMenu ;
    cm->Append ( POPUP_DUMMY , txt("p_dna_sequence") , dm ) ;
    dm->Append ( PC_ITEM_MARK , txt("p_item_mark") ) ;
    dm->Append ( PC_ITEM_MARK_SHOW , txt("p_item_mark_show") ) ;
    dm->AppendSeparator () ;
    dm->Append ( PC_ITEM_COPY_DNA , txt("p_item_copy_dna") ) ;
    dm->Append ( PC_ITEM_AS_NEW_SEQUENCE , txt("p_item_as_new_entry") ) ;
    dm->Append ( PC_ITEM_BLAST_DNA , txt("p_item_blast_dna") ) ;

    if ( rf != 0 )
        {
        wxMenu *am = new wxMenu ;
        cm->Append ( POPUP_DUMMY , txt("p_aa_sequence") , am ) ;
        am->Append ( PC_ITEM_COPY_AA , txt("p_item_copy_aa") ) ;
        am->Append ( PC_ITEM_AS_NEW_AA_SEQUENCE , txt("p_item_as_new_aa_entry") ) ;
        am->Append ( PC_ITEM_BLAST_AA , txt("p_item_blast_aa") ) ;
        }

    lastvectorobject = -1 ;
    if ( doreturn ) return cm ;
    PopupMenu ( cm , pt ) ;
    delete cm ;
    return NULL ;
    }
    
void PlasmidCanvas::itemMark ( wxCommandEvent &ev )
    {
    p->cSequence->mark ( "DNA" ,
                    p->vec->items[context_last_item].from ,
                    p->vec->items[context_last_item].to ) ;
    }
    
void PlasmidCanvas::itemMarkShow ( wxCommandEvent &ev )
    {
    itemMark ( ev ) ;
    p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;
    }
    
void PlasmidCanvas::itemEdit ( wxCommandEvent &ev )
    {
    invokeVectorEditor ( "item" , context_last_item ) ;
    }

void PlasmidCanvas::itemDelete ( wxCommandEvent &ev )
    {
    // Undo mechanism makes "Are you sure" question obsolete
/*    char t[1000] ;
    sprintf ( t , txt("t_item_delete") , p->vec->items[context_last_item].name.c_str() ) ;
    wxMessageDialog md ( this , t , txt("msg_box") , wxYES|wxNO ) ;
    if ( md.ShowModal() != wxID_YES ) return ;*/
    p->vec->undo.start ( txt("u_del_item") ) ;
    for ( int a = context_last_item+1 ; a < p->vec->items.size() ; a++ )
        p->vec->items[a-1] = p->vec->items[a] ;
    p->vec->items.pop_back () ;
    p->vec->recalcvisual = true ;
    p->vec->undo.stop() ;
    Refresh () ;
    p->updateSequenceCanvas ( true ) ;
    p->treeBox->initme() ;
    }

void PlasmidCanvas::itemShowHide ( wxCommandEvent &ev )
    {
    bool newstate = !p->vec->items[context_last_item].isVisible() ;
    p->vec->items[context_last_item].setVisible ( newstate ) ;
    Refresh () ;
    p->updateSequenceCanvas ( true ) ;
    p->treeBox->SetItemBold ( p->vec->items[context_last_item].treeid , newstate ) ;
//    p->treeBox->initme() ;
    }

// Restriction enzyme context menu
    
wxMenu *PlasmidCanvas::invokeRsPopup ( int rs , wxPoint pt , bool doreturn )
    {
    int a ;
    wxMenu *cm = new wxMenu ;
    context_last_rs = rs ;
    cm->Append ( PC_RS_EDIT , txt("p_rs_edit") ) ;
    cm->Append ( PC_RS_SHOW_HIDE , txt("m_show_hide_enzyme") ) ;
    cm->Append ( PC_RS_DEL , txt("p_rs_del") ) ;
    cm->Append ( -1 , "" ) ;
    cm->Append ( PC_RS_MARK , txt("p_rs_mark") ) ;
    cm->Append ( PC_RS_MARK_SHOW , txt("p_rs_mark_show") ) ;
    cm->Append ( PC_RS_INFO , txt("p_rs_info") ) ;
    cm->Append ( -1 , "" ) ;
    
    for ( a = 0 ; a < p->vec->cocktail.size() && 
                    p->vec->cocktail[a] != p->vec->rc[context_last_rs].e->name.c_str() ;
                    a++ ) ;
    if ( a == p->vec->cocktail.size() )
        {
        cm->Append ( PC_RS_ADD2COCKTAIL , txt("p_rs_add_to_cocktail") ) ;
        cm->Append ( PC_RS_ADD_AND_CUT , txt("p_rs_add_to_cocktail_and_cut") ) ;
        }
    else
        {
        cm->Append ( PC_RS_ADD2COCKTAIL , txt("p_rs_del_from_cocktail") ) ;
        }
    
    if ( p->vec->cocktail.size() > 0 )
        {
        cm->Append ( PC_RS_CUT_WITH_COCKTAIL , txt("p_cut_with_cocktail") ) ;
        cm->Append ( -1 , "" ) ;
        string c = txt("cocktail") ;
        c += " :" ;
        cm->Append ( PC_DUMMY , c.c_str() ) ;
        for ( a = 0 ; a < p->vec->cocktail.size() ; a++ )
            {
            c = "  " ;
            c += p->vec->cocktail[a] ;
            cm->Append ( PC_DUMMY , c.c_str() ) ;
            }
        }
        
    lastrestrictionsite = -1 ;
    if ( doreturn ) return cm ;
    PopupMenu ( cm , pt ) ;
    delete cm ;
    return NULL ;
    }
    
void PlasmidCanvas::rsMark ( wxCommandEvent &ev )
    {
    int from = p->vec->rc[context_last_rs].pos - 
                p->vec->rc[context_last_rs].e->cut + 1 ;
    int to = from + p->vec->rc[context_last_rs].e->sequence.length() - 1 ;
    p->cSequence->mark ( "DNA" , from , to ) ;
    }
    
void PlasmidCanvas::rsMarkShow ( wxCommandEvent &ev )
    {
    rsMark ( ev ) ;
    p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;    
    }
    
void PlasmidCanvas::rsAdd2Cocktail ( wxCommandEvent &ev )
    {
    int a , b ;
    for ( a = 0 ; a < p->vec->cocktail.size() && 
                    p->vec->cocktail[a] != p->vec->rc[context_last_rs].e->name.c_str() ;
                    a++ ) ;
    if ( a == p->vec->cocktail.size() ) // Add to cocktail
        {
        p->vec->cocktail.push_back ( p->vec->rc[context_last_rs].e->name.c_str() ) ;
        }
    else // Remove from cocktail
        {
        for ( b = a+1 ; b < p->vec->cocktail.size() ; b++ )
           p->vec->cocktail[b-1] = p->vec->cocktail[b] ;
        p->vec->cocktail.pop_back () ;
        }
    }
    
void PlasmidCanvas::rsCutWithCocktail ( wxCommandEvent &ev )
    {
    p->vec->setAction ( "RESTRICTION" , 20 ) ;
    p->vec->doAction() ;
    }
    
void PlasmidCanvas::rsAddAndCut ( wxCommandEvent &ev )
    {
    rsAdd2Cocktail ( ev ) ;
    rsCutWithCocktail ( ev ) ;
    }

void PlasmidCanvas::rsEdit ( wxCommandEvent &ev )
    {
    invokeVectorEditor ( "enzyme" , context_last_rs ) ;
    }
    
void PlasmidCanvas::rsInfo ( wxCommandEvent &ev )
    {
    string command = "http://rebase.neb.com/rebase/enz/" ;
    command += p->vec->rc[context_last_rs].e->name ;
    command += ".html" ;
    command = myapp()->getHTMLCommand ( command ) ;
    wxExecute ( command.c_str() ) ;
    }
    
void PlasmidCanvas::rsShowHide ( wxCommandEvent &ev )
    {
    p->treeBox->ToggleEnzymeVisibility ( p->vec->rc[context_last_rs].e ) ;
    }
    
void PlasmidCanvas::rsDel ( wxCommandEvent &ev )
    {
    p->vec->undo.start ( txt("u_del_enzyme") ) ;
    int a ;
    for ( a = 0 ; a < p->vec->cocktail.size() && 
                    p->vec->cocktail[a] != p->vec->rc[context_last_rs].e->name.c_str() ;
                    a++ ) ;
    if ( a < p->vec->cocktail.size() )
        rsAdd2Cocktail ( ev ) ; // Effectively removing enzyme from cocktail
        
    for ( a = 0 ; a < p->vec->re.size() &&
                        p->vec->re[a] != p->vec->rc[context_last_rs].e ; a++ ) ;
    for ( a++ ; a < p->vec->re.size() ; a++ )
        p->vec->re[a-1] = p->vec->re[a] ;
    p->vec->re.pop_back() ;
    p->vec->recalcvisual = true ;
    p->vec->recalculateCuts() ;
    p->treeBox->initme() ;
    p->treeBox->SelectItem ( p->treeBox->vroot ) ;
    p->cSequence->arrange() ;
    p->vec->undo.stop () ;
    Refresh () ;
    }

// Vector context menu

wxMenu *PlasmidCanvas::invokeVectorPopup ( wxPoint pt , bool doreturn )
    {
    wxMenu *cm = new wxMenu ;
    cm->Append ( PC_VECTOR_EDIT , txt("p_vector_edit") ) ;
    cm->Append(MDI_TRANSFORM_SEQUENCE, txt("t_transform_sequence") );
    
    if ( mark_from == -1 )
       {
       cm->Append(MDI_RUN_PCR, txt("m_pcr") );
       }
    else
       {
       wxMenu *pm = new wxMenu ;
       cm->Append ( POPUP_DUMMY , txt ( "m_pcr" ) , pm ) ;
       pm->Append(MDI_RUN_PCR, txt("m_pcr") );
       pm->Append ( PRIMER_FORWARD , txt("m_primer_forward") ) ;
       pm->Append ( PRIMER_BACKWARD , txt("m_primer_backward") ) ;
       pm->Append ( PRIMER_BOTH , txt("m_primer_both") ) ;
       if ( mark_from + 2 == mark_to )
          pm->Append ( PRIMER_MUTATION , txt("m_primer_mutation") ) ;
       }
    
    if ( p->vec->hasStickyEnds() && p->vec->isLinear() )
        cm->Append ( MDI_FILL_KLENOW , txt("p_fill_klenow") ) ;

    if ( mark_from != -1 )
        {
        wxMenu *mm = new wxMenu ;
        cm->Append ( POPUP_DUMMY , txt("p_selection") , mm ) ;
        mm->Append ( MDI_CUT , txt("m_cut") ) ;
        mm->Append ( MDI_COPY , txt("m_copy") ) ;
        mm->Append ( MDI_COPY_TO_NEW , txt("m_copy_to_new") ) ;
        mm->Append ( PC_WHAT_CUTS , txt("m_what_cuts") ) ;
        mm->Append ( MDI_AS_NEW_FEATURE , txt("m_as_new_feature") ) ;
        if ( p->aa_state != AA_NONE && p->aa_state != AA_ALL )
           {
           mm->Append ( MDI_EXTRACT_AA , txt("m_extract_aa") ) ;
           mm->Append ( PC_BLAST_AA , txt("m_blast_aa") ) ;
           }

        mm->Append ( PC_BLAST_DNA , txt("m_blast_dna") ) ;
        
        }
    if ( p->vec->cocktail.size() > 0 )
        {
        cm->Append ( PC_RS_CUT_WITH_COCKTAIL , txt("p_cut_with_cocktail") ) ;
        }
        
    if ( doreturn ) return cm ;
    cm->AppendSeparator();
    
    wxMenu *pm = new wxMenu ;
    cm->Append ( POPUP_DUMMY , txt("m_plasmid_map") , pm ) ;
    pm->Append ( PC_COPY_IMAGE , txt("m_copy_image") ) ;
    pm->Append ( MDI_PRINT_IMAGE , txt("m_print_image") ) ;
    
    cm->Append(MDI_ORFS, txt("m_orfs") );
    cm->Append(MDI_EDIT_ORFS, txt("m_edit_orfs") );

    PopupMenu ( cm , pt ) ;
    delete cm ;
    }

void PlasmidCanvas::vecEdit ( wxCommandEvent &ev )
    {
    invokeVectorEditor ( "" , 0 ) ;
    }
    
void PlasmidCanvas::blastDNA ( wxCommandEvent &ev )
    {
    if ( mark_from == -1 ) return ;
    string seq ;
    int a ;
    for ( a = mark_from ; a <= mark_to ; a++ )
        {
        seq += p->vec->getNucleotide ( a-1 ) ;
        }
    if ( seq == "" ) return ;
    myapp()->frame->blast ( seq , "blastn" ) ;
    }
        
void PlasmidCanvas::blastAA ( wxCommandEvent &ev )
    {
    string seq = p->doExtractAA() ;
    int a ;
    for ( a = 0 ; a < seq.size() && seq[a] == ' ' ; a++ ) ;
    if ( a == seq.size() )
       {
       wxMessageBox ( txt("t_no_or_empty_sequence") , 
                      txt("t_blast_failed") ,
                      wxOK | wxICON_ERROR  ) ;
       return ;
       }
    myapp()->frame->blast ( seq , "blastp" ) ;
    }

void PlasmidCanvas::RunPrimerEditor ( vector <TPrimer> &pl )
    {
    TPrimerDesign *subframe = new TPrimerDesign ( myapp()->frame , 
            txt("t_pcr") ,
            p->vec ,
            pl ) ;
    
    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    
    myapp()->frame->mainTree->addChild ( subframe , TYPE_PRIMER ) ;
    myapp()->frame->setChild ( subframe ) ;
//    myapp()->frame->children.push_back ( subframe ) ;
    }

// Primer handler

void PlasmidCanvas::OnPrimerForward ( wxCommandEvent &ev )
    {
    vector <TPrimer> pl ;
    int end = mark_from + 30 ;
    if ( end >= p->vec->sequence.length() ) end = p->vec->sequence.length() ;
    pl.push_back ( TPrimer ( mark_from , end , true ) ) ;
    RunPrimerEditor ( pl ) ;
    }

void PlasmidCanvas::OnPrimerBackward ( wxCommandEvent &ev )
    {
    vector <TPrimer> pl ;
    int start = mark_to - 30 ;
    if ( start < 1 ) start = 1 ;
    pl.push_back ( TPrimer ( start , mark_to , false ) ) ;
    RunPrimerEditor ( pl ) ;
    }

void PlasmidCanvas::OnPrimerBoth ( wxCommandEvent &ev )
    {
    vector <TPrimer> pl ;
    int end = mark_from + 30 ;
    if ( end >= p->vec->sequence.length() ) end = p->vec->sequence.length() ;
    pl.push_back ( TPrimer ( mark_from , end , true ) ) ;
    int start = mark_to - 30 ;
    if ( start < 1 ) start = 1 ;
    pl.push_back ( TPrimer ( start , mark_to , false ) ) ;
    RunPrimerEditor ( pl ) ;
    }

void PlasmidCanvas::OnPrimerMutation ( wxCommandEvent &ev )
    {
    vector <TPrimer> pl ;
    int start = mark_from - 10 ;
    int end = mark_from + 20 ;
    if ( end >= p->vec->sequence.length() ) end = p->vec->sequence.length() ;
    if ( start < 1 ) start = 1 ;
    pl.push_back ( TPrimer ( start , end , true ) ) ;
    
    start = mark_from - 20 ;
    end = mark_to + 10 ;
    if ( end >= p->vec->sequence.length() ) end = p->vec->sequence.length() ;
    if ( start < 1 ) start = 1 ;    
    pl.push_back ( TPrimer ( start , mark_to , false ) ) ;
    
    RunPrimerEditor ( pl ) ;
    }

// **** ORF popup menu handlers    

string PlasmidCanvas::getDNAorAA ( int from , int to , int dir , bool dna )
    {
    int a ;
    string r , s ;
    if ( to < from ) to += p->vec->sequence.length() ;
    
    for ( a = from ; a <= to ; a++ )
        {
        int b = a ;
        if ( a >= p->vec->sequence.length() ) b -= p->vec->sequence.length() ;
        r += p->vec->getNucleotide ( b ) ;
        }
    
    if ( dir < 0 )
        {
        s = "" ;
        for ( a = 0 ; a < r.length() ; a++ )
           s = p->vec->getComplement ( r[a] ) + s ;
        r = s ;
        }
    if ( !dna )
        {
        s = "" ;
        for ( a = 0 ; a+2 < r.length() ; a += 3 )
           {
           string t = p->vec->dna2aa ( r.substr ( a , 3 ) ) ;
           if ( t != "|" ) s += t ;
           }
        r = s ;
        }
    return r ;
    }

void PlasmidCanvas::orfCopyDNA ( wxCommandEvent &ev )
    {
    int from = p->vec->worf[context_last_orf].from ;
    int to = p->vec->worf[context_last_orf].to ;
    string s = getDNAorAA ( from , to , p->vec->worf[context_last_orf].rf ) ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }    
    }

void PlasmidCanvas::orfCopyAA ( wxCommandEvent &ev )
    {
    int from = p->vec->worf[context_last_orf].from ;
    int to = p->vec->worf[context_last_orf].to ;
    string s = getDNAorAA ( from , to , p->vec->worf[context_last_orf].rf , false ) ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }    
    }

void PlasmidCanvas::orfAsNewItem ( wxCommandEvent &ev )
    {
    char t[1000] ;
    int from = p->vec->worf[context_last_orf].from+1 ;
    int to = p->vec->worf[context_last_orf].to+1 ;

    if ( to > p->vec->sequence.length() )
        to -= p->vec->sequence.length() ;

    TVectorItem nvi ;
    sprintf ( t , txt("t_new_item_title") , from , to ) ;
    nvi.name = t ;
    nvi.direction = (p->vec->worf[context_last_orf].rf>0)?1:-1 ;
    nvi.type = VIT_CDS ;
    nvi.from = from ;
    nvi.to = to ;
    nvi.setParam ( "/codon_start" , 1 ) ;
    p->vec->undo.start ( txt("u_orf_new_item") ) ;
    p->vec->items.push_back ( nvi ) ;
    invokeVectorEditor ( "item" , p->vec->items.size()-1 , true ) ;
    p->vec->undo.stop() ;
    p->vec->recalcvisual = true ;
    Refresh () ;
    }

void PlasmidCanvas::orfAsNewDNA ( wxCommandEvent &ev )
    {
    TVector *nv = new TVector ;
    int from = p->vec->worf[context_last_orf].from ;
    int to = p->vec->worf[context_last_orf].to ;
    nv->sequence = getDNAorAA ( from , to , p->vec->worf[context_last_orf].rf ) ;
    nv->name = p->vec->name + " (" + string ( txt ( "t_orf_extracted" ) ) + ")" ;
    nv->desc = p->vec->desc + "\n" + string ( txt ( "t_orf_extracted" ) ) ;
    nv->setChanged () ;
    nv->setCircular ( false ) ;
    nv->recalculateCuts() ;
    nv->recalcvisual = true ;
    myapp()->frame->newFromVector ( nv ) ;
    }

void PlasmidCanvas::orfAsNewAA ( wxCommandEvent &ev )
    {
    int from = p->vec->worf[context_last_orf].from ;
    int to = p->vec->worf[context_last_orf].to ;
    string seq = getDNAorAA ( from , to , p->vec->worf[context_last_orf].rf , false ) ;
    string n = p->vec->name + " (" ;
    n += txt ( "t_orf_extracted" ) ;
    n += ")" ;
//    myapp()->frame->newAminoAcids ( seq , n ) ;
//    string s = getDNAorAA ( from , to , dir , false ) ;
//    string n = p->vec->items[context_last_item].name.c_str() ;
//    n += " (" + p->vec->name + ")" ;
    TVector *vvv = p->vec->getAAvector ( from+1 , to+1 , p->vec->worf[context_last_orf].rf ) ;
    TAminoAcids *aaa = myapp()->frame->newAminoAcids ( vvv , n ) ;
    aaa->vec->setChanged() ;
    }

void PlasmidCanvas::orfBlastDNA ( wxCommandEvent &ev )
    {
    int from = p->vec->worf[context_last_orf].from ;
    int to = p->vec->worf[context_last_orf].to ;
    string s = getDNAorAA ( from , to , p->vec->worf[context_last_orf].rf ) ;
    if ( s == "" ) return ;
    myapp()->frame->blast ( s , "blastn" ) ;
    }
        
void PlasmidCanvas::orfBlastAA ( wxCommandEvent &ev )
    {
    int from = p->vec->worf[context_last_orf].from ;
    int to = p->vec->worf[context_last_orf].to ;
    string s = getDNAorAA ( from , to , p->vec->worf[context_last_orf].rf , false ) ;
    if ( s == "" ) return ;
    myapp()->frame->blast ( s , "blastp" ) ;
    }

// *** More item handlers

void PlasmidCanvas::itemBlastDNA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    string s = getDNAorAA ( from , to , dir ) ;
    if ( s == "" ) return ;
    myapp()->frame->blast ( s , "blastn" ) ;
    }
        
void PlasmidCanvas::itemBlastAA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    int rf = p->vec->items[context_last_item].getRF() ;
    if ( rf == 0 ) return ;
    from += dir * ( rf - 1 ) ;
    to += dir * ( rf - 1 ) ;
    string s = getDNAorAA ( from , to , dir , false ) ;
    if ( s == "" ) return ;
    myapp()->frame->blast ( s , "blastp" ) ;
    }

void PlasmidCanvas::itemCopyAA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    int rf = p->vec->items[context_last_item].getRF() ;
    if ( rf == 0 ) return ;
    from += dir * ( rf - 1 ) - 1 ;
    to += dir * ( rf - 1 ) - 1 ;
    string s = getDNAorAA ( from , to , dir , false ) ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }    
    }

void PlasmidCanvas::itemAsNewAA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    int rf = p->vec->items[context_last_item].getRF() ;
    if ( rf == 0 ) return ;
    from += dir * ( rf - 1 ) - 1 ;
    to += dir * ( rf - 1 ) - 1 ;
//    string s = getDNAorAA ( from , to , dir , false ) ;
    string n = p->vec->items[context_last_item].name.c_str() ;
    n += " (" + p->vec->name + ")" ;
    TAminoAcids *aaa = myapp()->frame->newAminoAcids ( p->vec->getAAvector ( from+1 , to+1 , dir ) , n ) ;
    aaa->vec->setChanged() ;
    }
    
void PlasmidCanvas::itemAsNewSequence ( wxCommandEvent &ev )
    {
//    int from = p->vec->worf[context_last_orf].from ;
//    int to = p->vec->worf[context_last_orf].to ;
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    int rf = p->vec->items[context_last_item].getRF() ;

    if ( rf == 0 ) // No coding sequence, just copy the DNA and get outta here
        {
        itemMark ( ev ) ;
        p->OnCopyToNew ( ev ) ;
        }
    else // Coding sequence
        {
        from += dir * ( rf - 1 ) - 1 ;
        to += dir * ( rf - 1 ) - 1 ;
        string n = p->vec->items[context_last_item].name.c_str() ;
        n += " (" + p->vec->name + ")" ;
    
        TVector *nv = new TVector ;
        nv->sequence = getDNAorAA ( from , to , dir , true ) ;
        nv->name = n ;
        nv->setChanged () ;
        nv->setCircular ( false ) ;
        nv->recalculateCuts() ;
        nv->recalcvisual = true ;
        myapp()->frame->newFromVector ( nv ) ;
        }
    }    

void PlasmidCanvas::itemCopyDNA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    
    int rf = p->vec->items[context_last_item].getRF() ;
    if ( rf != 0 )
        {
        from += dir * ( rf - 1 ) - 1 ;
        to += dir * ( rf - 1 ) - 1 ;
        }

    string s = getDNAorAA ( from , to , dir ) ;

    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }    
    }

void PlasmidCanvas::OnFillKlenow(wxCommandEvent& event)
    {
    p->vec->undo.start ( txt("u_fill_klenow") ) ;
    TVector *v = p->vec ;
    string l = v->getStickyEnd(true,true) + v->getStickyEnd(true,false) ;
    string r = v->getStickyEnd(false,true) + v->getStickyEnd(false,false) ;
    int a ;
    if ( v->getStickyEnd(true,false) != "" )
        for ( a = 0 ; a < l.length() ; a++ )
           l[a] = v->getComplement ( l[a] ) ;
    if ( v->getStickyEnd(false,false) != "" )
        for ( a = 0 ; a < r.length() ; a++ )
           r[a] = v->getComplement ( r[a] ) ;
    v->sequence = l + v->sequence + r ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        v->items[a].from += l.length() ;
        v->items[a].to += l.length() ;
        }
    v->setStickyEnd ( true , true , "" ) ;
    v->setStickyEnd ( false , true , "" ) ;
    v->setStickyEnd ( true , false , "" ) ;
    v->setStickyEnd ( false , false , "" ) ;
    v->recalcvisual = true ;
    v->recalculateCuts() ;
    Refresh () ;
    SeqDNA *dna = (SeqDNA*) p->cSequence->findID ( "DNA" ) ;
    dna->initFromTVector ( v ) ;
    p->updateSequenceCanvas() ;
    p->cSequence->arrange () ;
    p->vec->undo.stop () ;
    p->cSequence->Refresh () ;
    p->treeBox->initme() ;
    }

void PlasmidCanvas::OnRunPCR(wxCommandEvent& event)
    {
    vector <TPrimer> pl ;
    RunPrimerEditor ( pl ) ;
    }

void PlasmidCanvas::setZoom ( int factor )
    {
    zoom = factor ;
    p->vec->recalcvisual = true ;
    int vx , vy ;
    GetClientSize ( &vx , &vy ) ;
    vx = vx * factor / 100 ;
    vy = vy * factor / 100 ;
    SetScrollbars ( 1 , 1 , vx , vy , false ) ;
    SetVirtualSize ( vx , vy ) ;
    Refresh () ;
    }
    
void PlasmidCanvas::OnWhatCuts(wxCommandEvent& event)
    {
    p->cSequence->OnWhatCuts(event) ;
    }
    
