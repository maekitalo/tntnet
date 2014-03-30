XPTemplate priority=personal

let s:f = g:XPTfuncs()

XPTinclude
      \ _common/common

let s:nIndent = 0
fun! s:f.ecpp_cont_ontype()
    let v = self.V()

    if v =~ '\V\n'
        let v = matchstr( v, '\V\.\*\S\ze\s\*\n' )

        let s:nIndent = &indentexpr != ''
              \ ? eval( substitute( &indentexpr, '\Vv:lnum', 'line(".")', '') ) - indent( line( "." ) - 1 )
              \ : self.NIndent()

        return self.FinishOuter( v . "\n" . repeat( '', s:nIndent ) )
    else
        return v
    endif

endfunction

fun! s:f.ecpp_cont_helper()
    let v = self.V()

    if v =~ '\V\n'
        return self.ResetIndent( -s:nIndent, "\n" )
    else
        return matchstr( v, '\V\^\s\*' )
    endif

endfunction

XPT #
XSET content|ontype=ecpp_cont_ontype()
<#` `content^^`content^ecpp_cont_helper()^#>

XPT $ " <$ expr $>
<$ `expr^ $>

XPT $$ " <$$ expr $>
<$$ `expr^ $>

XPT ? " <? cond ? expr ?>
<? `cond^ ? `expr^ ?>

XPT ?? " <?? cond ? expr ?>
<?? `cond^ ? `expr^ ?>

XPT & " <& component [ arguments ] >
<& `component^` `arguments^ &>

XPT <{ " <{...}>
<{
`cursor^
}>

XPT app " <%application>...</application>
<%application` scope="`scope`"^>
`cursor^
</%application>
XSET scope=ChooseStr('component', 'page', 'shared')
XSET scope|post=Echo( V() =~ '""' ? "" : V() )

XPT arg " <%args>...</%args>
<%args>
`cursor^
</%args>

XPT conf " <%config>...</%config>
<%config>
`cursor^
</%config>

XPT cpp " <%cpp>...</%cpp>
<%cpp>
`cursor^
</%cpp>

XPT def " <%def>...</%def>
<%def `name^>
`cursor^
</%def>

XPT doc " <%doc>...</%doc>
<%doc>
`cursor^
</%doc>

XPT i18n " <%i18n>...</%i18n>
<%i18n>
`cursor^
</%i18n>

XPT inc " <%include>...</%include>
<%include>
`cursor^
</%include>

XPT par " <%param>...</%param>
<%param` scope="`scope`"^>
`cursor^
</%param>
XSET scope=ChooseStr('component', 'page', 'shared')
XSET scope|post=Echo( V() =~ '""' ? "" : V() )

XPT pre " <%pre>...</%pre>
<%pre>
`cursor^
</%pre>

XPT req " <%request>...</%request>
<%request` scope="`scope`"^>
`cursor^
</%request>
XSET scope=ChooseStr('component', 'page', 'shared')
XSET scope|post=Echo( V() =~ '""' ? "" : V() )

XPT ses " </session>...</%session>
<%session` scope="`scope`"^>
`cursor^
</%session>
XSET scope=ChooseStr('component', 'page', 'shared')
XSET scope|post=Echo( V() =~ '""' ? "" : V() )

XPT thr " </thread>...</%thread>
<%thread` scope="`scope`"^>
`cursor^
</%thread>
XSET scope=ChooseStr('component', 'page', 'shared')
XSET scope|post=Echo( V() =~ '""' ? "" : V() )
