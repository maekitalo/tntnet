XPTemplate priority=personal

XPTinclude
    \ _common/common

XPT $ " <$ expr $>
<$ `expr^ $>

XPT $$ " <$$ expr $>
<$$ `expr^ $>

XPT # " <#...#>
<# `comment^ #>

XPT ? " <? cond ? expr ?>
<? `cond^ ? `expr^ ?>

XPT ?? " <?? cond ? expr ?>
<?? `cond^ ? `expr^ ?>

XPT com " <& component [ arguments ] >
<& `component^` `arguments^ >

XPT <{ " <{...}>
<{
`cursor^
}>

XPT app
<%application`scope^>
`cursor^
</%application>
XSET scope=ChooseStr(' scope="component"', ' scope="page"', ' scope="global"')

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

XPT par
<%param`scope^>
`cursor^
</%param>
XSET scope=ChooseStr(' scope="component"', ' scope="page"', ' scope="global"')

XPT pre " <%pre>...</%pre>
<%pre>
`cursor^
</%pre>

XPT req
<%request`scope^>
`cursor^
</%request>
XSET scope=ChooseStr(' scope="component"', ' scope="page"', ' scope="global"')

XPT ses
<%session`scope^>
`cursor^
</%session>
XSET scope=ChooseStr(' scope="component"', ' scope="page"', ' scope="global"')

XPT thr
<%thread`scope^>
`cursor^
</%thread>
XSET scope=ChooseStr(' scope="component"', ' scope="page"', ' scope="global"')
