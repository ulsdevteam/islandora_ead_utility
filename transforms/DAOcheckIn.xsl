<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ns2="http://www.w3.org/1999/xlink"
 xpath-default-namespace="urn:isbn:1-931666-22-9"
 xmlns:ead="urn:isbn:1-931666-22-9"
 xmlns:xlink="http://www.w3.org/1999/xlink" version="1.0">
    <xsl:output method="text" indent="no" encoding="utf-8"/>
<xsl:variable name="decrement">1</xsl:variable>
<xsl:template match="/">
    <xsl:text>id&#09;name&#09;ead_id&#10;</xsl:text>
    <xsl:apply-templates select="//ead:c01 | //ead:c02 | //ead:c03 | //ead:c04 | //ead:c05 | //ead:c06 | //ead:c07 | //ead:c08 | //ead:c09" />
</xsl:template>
    <xsl:template match="//ead:c01 | //ead:c02 | //ead:c03 | //ead:c04 | //ead:c05 | //ead:c06 | //ead:c07 | //ead:c08 | //ead:c09">
        <xsl:param name="containers" select="//ead:c01 | //ead:c02 | //ead:c03 | //ead:c04 | //ead:c05 | //ead:c06 | //ead:c07 | //ead:c08 | //ead:c09" />
        <xsl:param name="position" select="position()" />
        <xsl:for-each select="ead:dao">
            <xsl:value-of select="normalize-space(@xlink:href)" />
            <xsl:text>&#09;</xsl:text>
            <xsl:for-each select="../../ead:did/ead:container | ../ead:did/ead:container">
                <xsl:if test="@type='file' or @type='folder' or @type='Folder' or @type='Folders' or @type='Item' or @type='item' or @type='oversize' or @type='Oversize' or @type='volume' or @type='Volume' or @type='Volumes'">
                    <xsl:value-of select="normalize-space(//ead:eadid)" />
                    <xsl:text> </xsl:text>
                    <xsl:text>Box </xsl:text>
                    <xsl:call-template name="box_recurse">
                        <xsl:with-param name="the_c" select="$containers" />
                        <xsl:with-param name="index" select="$position" />
                    </xsl:call-template>
                    <xsl:value-of select="@type" />
                    <xsl:text> </xsl:text>
                    <xsl:value-of select="."/>
                    <xsl:text> </xsl:text>
                </xsl:if>
            </xsl:for-each>
                    <xsl:value-of select="normalize-space(../ead:did/ead:unittitle)" />
                    <xsl:text> </xsl:text>
            <xsl:value-of select="../ead:did/ead:unitdate" />
            <!-- test to account for duplicate daodesc title where there is only one dao per folder -->
            <!--
            <xsl:if test="normalize-space(concat(../ead:did/ead:unittitle, ', ',  ../ead:did/ead:unitdate))!=normalize-space(ead:daodesc)">
                <xsl:if test="normalize-space(../ead:did/ead:unittitle)!=normalize-space(ead:daodesc)">
                    <xsl:if test="normalize-space(concat(../ead:did/ead:unittitle,  ../ead:did/ead:unitdate))=normalize-space(ead:daodesc)">
                    <xsl:text>, </xsl:text>
                    <xsl:value-of select="normalize-space(ead:daodesc)" />
                        </xsl:if>
                </xsl:if>
            </xsl:if>
            -->
            <xsl:text>&#09;</xsl:text>
            <xsl:value-of select="normalize-space(//ead:eadid)" />
            <xsl:text>&#10;</xsl:text>
        </xsl:for-each>
    </xsl:template>


    <xsl:template name="box_recurse">
        <xsl:param name="the_c" />
        <xsl:param name="index" />
        <!-- <xsl:value-of select="$index" /> -->
        <xsl:choose>
            <xsl:when test="$the_c[$index]/ead:did/ead:container[@type='Box']">
                <xsl:value-of select="$the_c[$index]/ead:did/ead:container[@type='Box']"/>
                <xsl:text> </xsl:text>
            </xsl:when>
            <xsl:when test="$index - $decrement != 0">
                <xsl:call-template name="box_recurse">
                    <xsl:with-param name="the_c" select="$the_c" />
                    <xsl:with-param name="index" select="$index - $decrement" />
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>0</xsl:text>
            </xsl:otherwise>
        </xsl:choose>

    </xsl:template>
</xsl:stylesheet>