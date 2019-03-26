<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:copyrightMD="http://www.cdlib.org/inside/diglib/copyrightMD">
  <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes" />
    <xsl:template match="//mods:mods">
       <xsl:copy-of select="." />
    </xsl:template>
</xsl:stylesheet>