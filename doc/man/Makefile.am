dist_man_MANS = \
	tntnet.8 \
	ecppc.1 \
	ecpp.7 \
	tntnet-defcomp.1 \
	tntnet.xml.7

if GENERATE_MAN
tntnet.8: tntnet.8.markdown
	$(MD2MAN) -in $< -out $@

ecppc.1: ecppc.1.markdown
	$(MD2MAN) -in $< -out $@

ecpp.7: ecpp.7.markdown
	$(MD2MAN) -in $< -out $@

tntnet-defcomp.1: tntnet-defcomp.1.markdown
	$(MD2MAN) -in $< -out $@

tntnet.xml.7: tntnet.xml.7.markdown
	$(MD2MAN) -in $< -out $@
endif
