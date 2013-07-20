" Vim syntax file
" Language:    ECPP
" Maintainer:  Tommi Maekitalo <tommi@tntnet.org>
" Last change: 2003 Sep 10
" URL:         http://www.maekitalo.de/vim/ecpp.vim
"
"
if version < 600
	syn clear
elseif exists("b:current_syntax")
	finish
endif

" The HTML syntax file included below uses this variable.
"
if !exists("main_syntax")
	let main_syntax = 'ecpp'
endif

" First pull in the HTML syntax.
"
if version < 600
	so <sfile>:p:h/html.vim
else
	runtime! syntax/html.vim
	unlet b:current_syntax
endif

syn cluster htmlPreproc add=@ecppTop

" Now pull in the cpp syntax.
"
if version < 600
	syn include @cppTop <sfile>:p:h/cpp.vim
else
	syn include @cppTop syntax/cpp.vim
endif

" It's hard to reduce down to the correct sub-set of Cpp to highlight in some
" of these cases so I've taken the safe option of just using cppTop in all of
" them. If you have any suggestions, please let me know.
"

syn match ecppCondExprDelim "?"

" Eat opening braces when starting c++-regions - I don't exactly know, why
" this is needed - it just works better
syn region ecppLine matchgroup=Delimiter start="^%" end="{*\s*$" contains=@cppTop
syn region ecppExpr matchgroup=Delimiter start="<\$" end="{*\s*\$>" contains=@cppTop
syn region ecppCondExpr matchgroup=Delimiter start="<??\?" end="?>" contains=ecppCondExprDelim,@cppTop
syn region ecppCpp matchgroup=Delimiter start="<%cpp>" end="{*\s*</%cpp>" contains=@cppTop
syn region ecppCpps matchgroup=Delimiter start="<{" end="}>" contains=@cppTop
syn region ecppComp keepend matchgroup=Delimiter start="<&" end=">" contains=@cppTop
syn region ecppEndComp keepend matchgroup=Delimiter start="</&" end=">" contains=@cppTop

syn region ecppArgs matchgroup=Delimiter start="<%args>" end="</%args>" contains=@cppTop
syn region ecppGet matchgroup=Delimiter start="<%get>" end="</%get>" contains=@cppTop
syn region ecppPost matchgroup=Delimiter start="<%post>" end="</%post>" contains=@cppTop
syn region ecppConfig matchgroup=Delimiter start="<%config>" end="</%config>" contains=@cppTop
syn region ecppAttr matchgroup=Delimiter start="<%attr>" end="</%attr>" contains=@cppTop
syn region ecppVar matchgroup=Delimiter start='<%\z(application\|session\|securesession\|request\|thread\)\s*\n*\s*\(scope\s*=\s*"\(shared\|global\|page\|component\)"\)\?\s*\n*\s*\(include\s*=\s*".*"\s*\n*\s*\)*\s*>' end='</%\z1>' contains=@cppTop
syn region ecppVar matchgroup=Delimiter start="<%param>" end="</%param>" contains=@cppTop

syn region ecppInit matchgroup=Delimiter start="<%init>" end="</%init>" contains=@cppTop
syn region ecppPre matchgroup=Delimiter start="<%pre>" end="</%pre>" contains=@cppTop
syn region ecppCleanup matchgroup=Delimiter start="<%cleanup>" end="</%cleanup>" contains=@cppTop
syn region ecppShared matchgroup=Delimiter start="<%shared>" end="</%shared>" contains=@cppTop
syn region ecppClose matchgroup=Delimiter start="<%close>" end="</%close>" contains=@cppTop
syn region ecppSout matchgroup=Delimiter start="<%out>" end="</%out>" contains=@cppTop
syn region ecppOut matchgroup=Delimiter start="<%sout>" end="</%sout>" contains=@cppTop
syn region ecppIncluded display contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match ecppIncluded display contained "<[^>]*>"
syn region ecppInclude matchgroup=Delimiter start="<%include>" end="</%include>" contains=@ecppIncluded

" syn region ecppMethod matchgroup=Delimiter start="<%method[^>]*>" end="</%method>" contains=@htmlTop

syn region ecppDoc matchgroup=Delimiter start="<%doc>" end="</%doc>"
syn region ecppComment matchgroup=Delimiter start="<#" end="#>" contains=@cCommentGroup
syn region ecppCommentm matchgroup=Delimiter start="<%doc>" end="</%doc>" contains=@cCommentGroup
syn region ecppTranslateTag matchgroup=Delimiter start="{" end="}"
" syn match ecppTranslate contained "[^}]\+"

syn cluster ecppTop contains=ecppLine,ecppExpr,ecppCondExpr,ecppCpp,ecppCpps,ecppComp,ecppEndComp,ecppArgs,ecppAttr,ecppConfig,ecppVar,ecppInit,ecppPre,ecppCleanup,ecppShared,ecppDoc,ecppComment,ecppCommentm,ecppTranslateTag,ecppInclude

syn region ecppDef matchgroup=Delimiter start="<%def[^>]*>" end="</%def>" contains=@htmlTop

" Set up default highlighting. Almost all of this is done in the included
" syntax files.
"
if version >= 508 || !exists("did_ecpp_syn_inits")
	if version < 508
		let did_ecpp_syn_inits = 1
		com -nargs=+ HiLink hi link <args>
	else
		com -nargs=+ HiLink hi def link <args>
	endif

	HiLink ecppDoc Comment
	HiLink ecppComment Comment
	HiLink ecppCommentm Comment
	HiLink ecppTranslateTag Identifier
    HiLink ecppIncluded		String

	delc HiLink
endif

let b:current_syntax = "ecpp"

if main_syntax == 'ecpp'
	unlet main_syntax
endif
