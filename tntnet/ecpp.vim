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

syn region ecppLine matchgroup=Delimiter start="^%" end="$" contains=@cppTop
syn region ecppExpr matchgroup=Delimiter start="<\$" end="\$>" contains=@cppTop
syn region ecppCondExpr matchgroup=Delimiter start="<?" end="?>" contains=ecppCondExprDelim,@cppTop
syn region ecppCpp matchgroup=Delimiter start="<%cpp>" end="</%cpp>" contains=@cppTop
syn region ecppCpps matchgroup=Delimiter start="<{" end="}>" contains=@cppTop
syn region ecppComp keepend matchgroup=Delimiter start="<&" end="&>" contains=@cppTop

syn region ecppArgs matchgroup=Delimiter start="<%args>" end="</%args>" contains=@cppTop
syn region ecppConfig matchgroup=Delimiter start="<%config>" end="</%config>" contains=@cppTop
syn region ecppAttr matchgroup=Delimiter start="<%attr>" end="</%attr>" contains=@cppTop
syn region ecppVar matchgroup=Delimiter start="<%var>" end="</%var>" contains=@cppTop
syn region ecppVar matchgroup=Delimiter start="<%applicationScope>" end="</%applicationScope>" contains=@cppTop
syn region ecppVar matchgroup=Delimiter start="<%sessionScope>" end="</%sessionScope>" contains=@cppTop
syn region ecppVar matchgroup=Delimiter start="<%requestScope>" end="</%requestScope>" contains=@cppTop
syn region ecppVar matchgroup=Delimiter start="<%pageScope>" end="</%pageScope>" contains=@cppTop
syn region ecppVar matchgroup=Delimiter start="<%componentScope>" end="</%componentScope>" contains=@cppTop

syn region ecppInit matchgroup=Delimiter start="<%init>" end="</%init>" contains=@cppTop
syn region ecppPre matchgroup=Delimiter start="<%pre>" end="</%pre>" contains=@cppTop
syn region ecppGlobal matchgroup=Delimiter start="<%global>" end="</%global>" contains=@cppTop
syn region ecppDeclare matchgroup=Delimiter start="<%declare>" end="</%declare>" contains=@cppTop
syn region ecppDeclareShared matchgroup=Delimiter start="<%declare_shared>" end="</%declare_shared>" contains=@cppTop
syn region ecppDefine matchgroup=Delimiter start="<%define>" end="</%define>" contains=@cppTop
syn region ecppCleanup matchgroup=Delimiter start="<%cleanup>" end="</%cleanup>" contains=@cppTop
" syn region ecppOnce matchgroup=Delimiter start="<%once>" end="</%once>" contains=@cppTop
syn region ecppShared matchgroup=Delimiter start="<%shared>" end="</%shared>" contains=@cppTop
" syn region	ecppIncluded	display contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match	ecppIncluded	display contained "<[^>]*>"
syn region ecppInclude matchgroup=Delimiter start="<%include>" end="</%include>" contains=@ecppIncluded

syn region ecppDef matchgroup=Delimiter start="<%def[^>]*>" end="</%def>" contains=@htmlTop
" syn region ecppMethod matchgroup=Delimiter start="<%method[^>]*>" end="</%method>" contains=@htmlTop

syn region ecppDoc matchgroup=Delimiter start="<%doc>" end="</%doc>"
syn region ecppComment matchgroup=Delimiter start="<#" end="#>" contains=@cCommentGroup
syn region ecppTranslateTag matchgroup=Delimiter start="{" end="}"
" contains=ecppTranslate
" syn match ecppTranslate contained "[^}]\+"
" syn region ecppText matchgroup=Delimiter start="<%text>" end="</%text>"

" syn region ecppClass matchgroup=Delimiter start="<%class>" end="</%class>" contains=@cppTop

syn cluster ecppTop contains=ecppLine,ecppExpr,ecppCondExpr,ecppCpp,ecppCpps,ecppComp,ecppArgs,ecppAttr,ecppConfig,ecppVar,ecppInit,ecppInit,ecppCleanup,ecppShared,ecppDef,ecppDoc,ecppText,ecppGlobal,ecppDeclare,ecppDeclareShared,ecppDefine,ecppComment,ecppTranslateTag,ecppTranslate

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
	HiLink ecppTranslateTag Identifier
	HiLink ecppTranslate Identifier

	delc HiLink
endif

let b:current_syntax = "ecpp"

if main_syntax == 'ecpp'
	unlet main_syntax
endif
