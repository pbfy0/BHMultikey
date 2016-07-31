$BrawlhallaPath = "C:\Program Files (x86)\Steam\steamapps\common\Brawlhalla\"

echo "Copying mod"
cp MultiKeyboard.swf $BrawlhallaPath\mods\
if(!(Test-Path $BrawlhallaPath\META-INF\AIR\extensions\MultiKeyboard\ -pathType container)) {
	echo "Copying ANE"
	cp -r MultiKeyboard $BrawlhallaPath\META-INF\AIR\extensions\
}
$xmlpath = "$BrawlhallaPath\META-INF\AIR\application.xml"
[xml]$ApplicationXml = Get-Content -Path $BrawlhallaPath\META-INF\AIR\application.xml
if(!($ApplicationXml.application.extensions.extensionID.Contains("MultiKeyboard"))) {
	echo "Patching XML"
	cp $xmlpath $xmlpath.mk_bak
	$ee = $ApplicationXml.CreateElement("extensionID", $ApplicationXml.application.NamespaceURI)
	$ee.InnerText = "MultiKeyboard"
	$ApplicationXml.application.extensions.AppendChild($ee)
	$ApplicationXml.save($xmlpath)
}
echo "BHMultikey installation complete"