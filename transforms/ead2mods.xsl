<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.loc.gov/mods/v3"
    xmlns:ns2="http://www.w3.org/1999/xlink"
    xmlns:ead="urn:isbn:1-931666-22-9"
    xpath-default-namespace="urn:isbn:1-931666-22-9" version="1.0">
    <xsl:output method="xml" indent="yes" encoding="utf-8"/>
    <xsl:strip-space elements="*"/>
    <xsl:variable name="decrement">1</xsl:variable>
    <xsl:template match="/">
        <xsl:apply-templates
            select="//ead:c01 | //ead:c02 | //ead:c03 | //ead:c04 | //ead:c05 | //ead:c06 | //ead:c07 | //ead:c08 | //ead:c09"/>
    </xsl:template>
    <xsl:template match="//ead:c01 | //ead:c02 | //ead:c03 | //ead:c04 | //ead:c05 | //ead:c06 | //ead:c07 | //ead:c08 | //ead:c09">
        <xsl:param name="containers"
            select="//ead:c01 | //ead:c02 | //ead:c03 | //ead:c04 | //ead:c05 | //ead:c06 | //ead:c07 | //ead:c08 | //ead:c09"/>
        <xsl:param name="position" select="position()"/>
        <xsl:for-each select="ead:dao">
            <xsl:variable name="barcode" select="normalize-space(@xlink:href)"/>
            <xsl:variable name="filename" select="concat($barcode,'_MODS.xml')"/>

            <xsl:text>filename=</xsl:text><xsl:value-of select="$filename" /><xsl:text>&#10;</xsl:text>
                <!-- <xsl:result-document href="{$filename}"> -->
                <mods:mods
                    xsi:schemaLocation="http://www.loc.gov/mods/v3 http://www.loc.gov/standards/mods/v3/mods-3-3.xsd"
                    xmlns:xlink="http://www.w3.org/1999/xlink"
                    xmlns:mods="http://www.loc.gov/mods/v3"
                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
                    <mods:titleInfo>
                        <mods:title>
                            <xsl:value-of select="normalize-space(../ead:did/ead:unittitle)"/>
                        </mods:title>
                        <!-- test to account for duplicate daodesc title where there is only one dao per folder -->
                       <!-- <xsl:if
                            test="normalize-space(concat(../ead:did/ead:unittitle, ', ',  ../ead:did/ead:unitdate))!=normalize-space(ead:daodesc)">
                            <xsl:if
                                test="normalize-space(../ead:did/ead:unittitle)!=normalize-space(ead:daodesc)">
                                <xsl:if
                                    test="normalize-space(concat(../ead:did/ead:unittitle,  ../ead:did/ead:unitdate))=normalize-space(ead:daodesc)">
                                    <mods:subTitle>
                                        <xsl:value-of select="normalize-space(ead:daodesc)"/>
                                    </mods:subTitle>
                                </xsl:if>
                            </xsl:if>
                        </xsl:if> -->
                    </mods:titleInfo>
                    <mods:name>
                        <mods:namePart>University of Pittsburgh</mods:namePart>
                        <mods:role>
                            <mods:roleTerm type="text">depositor</mods:roleTerm>
                        </mods:role>
                    </mods:name>
                    <mods:typeOfResource>text</mods:typeOfResource>
                    <mods:genre>archival document</mods:genre>
                    <mods:originInfo>
                        <mods:dateOther type="display">
                            <xsl:value-of select="preceding-sibling::ead:did/ead:unitdate"/>
                        </mods:dateOther>
                    </mods:originInfo>
                    <mods:identifier type="pitt">
                        <xsl:value-of select="normalize-space(@xlink:href)"/>
                    </mods:identifier>
                    <mods:relatedItem type="host">
                        <mods:titleInfo>
                            <mods:title>
                                <xsl:value-of select="normalize-space(//ead:archdesc/ead:did/ead:unittitle)"/>
                            </mods:title>
                        </mods:titleInfo>
                        <mods:identifier type="local-asc">
                            <xsl:value-of select="//ead:archdesc/ead:did/ead:unitid"/>
                        </mods:identifier>
                        <mods:originInfo>
                            <mods:dateCreated>
                                <xsl:value-of select="//ead:archdesc/ead:did/ead:unitdate"/>
                            </mods:dateCreated>
                        </mods:originInfo>
                        <xsl:if test="ancestor::*/@level='series' ">
                            <mods:note type="series">
                                <xsl:value-of select="ancestor::*[@level='series']/ead:did/ead:unitid"/>
                                <xsl:text> </xsl:text>
                                <xsl:value-of select="ancestor::*[@level='series']/ead:did/ead:unittitle"/>
                                <xsl:text> </xsl:text>
                                <xsl:value-of select="ancestor::*[@level='series']/ead:did/ead:unitdate"/>
                            </mods:note>
                        </xsl:if>
                        <xsl:if test="ancestor::*/@level='subseries'">
                            <mods:note type="subseries">
                                <xsl:value-of select="ancestor::*[@level='subseries']/ead:did/ead:unitid"/>
                                <xsl:text> </xsl:text>
                                <xsl:value-of select="ancestor::*[@level='subseries']/ead:did/ead:unittitle"/>
                                <xsl:text> </xsl:text>
                                <xsl:value-of select="ancestor::*[@level='subseries']/ead:did/ead:unitdate"
                                />
                            </mods:note>
                        </xsl:if>
                        <xsl:if test="ancestor::*/@level='otherlevel'">
                            <mods:note type="otherlevel">
                                <xsl:value-of
                                    select="ancestor::*[@level='otherlevel']/ead:did/ead:unittitle"/>
                            </mods:note>
                        </xsl:if>
                        <xsl:choose>
                            <xsl:when test="../ead:did/ead:container">
                                <xsl:for-each select="../ead:did/ead:container">
                                    <xsl:if test="@type='file' or @type='folder' or @type='Folder' or @type='Folders' or @type='Item' or @type='item' or @type='oversize' or @type='Oversize' or @type='volume' or @type='Volume' or @type='Volumes'">
                                        <!-- <xsl:if test=".[@type!='Audio-cabinet'] |.[@type!='Boxes'] | .[@type!='Box'] | .[@type!='Drawer'] | .[@type!='File-cabinet'] | .[@type!='Map-case'] | .[@type!='Microfiche-cabinet'] | .[@type!='Microfilm-cabinet'] | .[@type!='Museum-cabinet'] | .[@type!='Safe'] | .[@type!='Shelf']">
                                 -->
                                        <mods:note type="container">
                                            <xsl:call-template name="box_recurse">
                                                <xsl:with-param name="the_c" select="$containers"/>
                                                <xsl:with-param name="index" select="$position"/>
                                            </xsl:call-template>
                                            <xsl:value-of select="@type"/>
                                            <xsl:text> </xsl:text>
                                            <xsl:value-of select="."/>
                                        </mods:note>
                                    </xsl:if>
                                </xsl:for-each>
                            </xsl:when>
                            <xsl:otherwise>
                                <mods:note type="container">
                                    <xsl:call-template name="container_recurse">
                                        <xsl:with-param name="the_c" select="$containers"/>
                                        <xsl:with-param name="index" select="$position"/>
                                    </xsl:call-template>
                                </mods:note>
                            </xsl:otherwise>
                        </xsl:choose>
                        <mods:note type="ownership">
                            <xsl:value-of select="normalize-space(//ead:archdesc/ead:did/ead:repository)"/>
                        </mods:note>
                        <mods:note type="prefercite">
                            <xsl:value-of select="//ead:prefercite/ead:p"/>
                        </mods:note>
                    </mods:relatedItem>
                </mods:mods>
            <!-- </xsl:result-document> -->
            <xsl:text>[[END]]</xsl:text>
        </xsl:for-each>
    </xsl:template>

    <xsl:template name="container_recurse">
        <xsl:param name="the_c"/>
        <xsl:param name="index"/>
        <xsl:param name="position" select="position()"/>
        <!-- <xsl:value-of select="$index" /> -->
        <xsl:choose>
            <xsl:when test="$the_c[$index]/ead:did/ead:container[last()] ">
                <xsl:call-template name="box_recurse">
                    <xsl:with-param name="the_c" select="$the_c"/>
                    <xsl:with-param name="index" select="$position"/>
                </xsl:call-template>
                <xsl:value-of select="@type"/>
                <xsl:value-of select="$the_c[$index]/ead:did/ead:container[last()]/@type"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="$the_c[$index]/ead:did/ead:container[last()]"/>
            </xsl:when>
            <xsl:when test="$index - $decrement != 0">
                <xsl:call-template name="container_recurse">
                    <xsl:with-param name="the_c" select="$the_c"/>
                    <xsl:with-param name="index" select="$index - $decrement"/>
                </xsl:call-template>
            </xsl:when>
        </xsl:choose>

    </xsl:template>

    <xsl:template name="box_recurse">
        <xsl:param name="the_c"/>
        <xsl:param name="index"/>
        <!-- <xsl:value-of select="$index" /> -->
        <xsl:choose>
            <xsl:when
                test="$the_c[$index]/ead:did/ead:container[@type='Box']  | $the_c[$index]/ead:did/ead:container[@type='Map-case'] |  $the_c[$index]/ead:did/ead:container[@type='Audio-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Boxes'] | $the_c[$index]/ead:did/ead:container[@type='Drawer'] | $the_c[$index]/ead:did/ead:container[@type='File-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Microfiche-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Microfilm-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Museum-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Safe'] | $the_c[$index]/ead:did/ead:container[@type='Shelf']">
                <xsl:value-of
                    select="$the_c[$index]/ead:did/ead:container[@type='Box']/@type  | $the_c[$index]/ead:did/ead:container[@type='Map-case']/@type  | $the_c[$index]/ead:did/ead:container[@type='Audio-cabinet']/@type  | $the_c[$index]/ead:did/ead:container[@type='Boxes']/@type  | $the_c[$index]/ead:did/ead:container[@type='Drawer']/@type  | $the_c[$index]/ead:did/ead:container[@type='File-cabinet']/@type  | $the_c[$index]/ead:did/ead:container[@type='Microfiche-cabinet']/@type  | $the_c[$index]/ead:did/ead:container[@type='Microfilm-cabinet']/@type  | $the_c[$index]/ead:did/ead:container[@type='Museum-cabinet']/@type  | $the_c[$index]/ead:did/ead:container[@type='Safe']/@type  | $the_c[$index]/ead:did/ead:container[@type='Shelf']/@type"/>
                <xsl:text> </xsl:text>
                <xsl:value-of
                    select="$the_c[$index]/ead:did/ead:container[@type='Box']  | $the_c[$index]/ead:did/ead:container[@type='Map-case'] | $the_c[$index]/ead:did/ead:container[@type='Audio-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Boxes'] | $the_c[$index]/ead:did/ead:container[@type='Drawer'] | $the_c[$index]/ead:did/ead:container[@type='File-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Microfiche-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Microfilm-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Museum-cabinet'] | $the_c[$index]/ead:did/ead:container[@type='Safe'] | $the_c[$index]/ead:did/ead:container[@type='Shelf']"/>
                <xsl:text>, </xsl:text>
            </xsl:when>
            <xsl:when test="$index - $decrement != 0">
                <xsl:call-template name="box_recurse">
                    <xsl:with-param name="the_c" select="$the_c"/>
                    <xsl:with-param name="index" select="$index - $decrement"/>
                </xsl:call-template>
            </xsl:when>
        </xsl:choose>

    </xsl:template>
</xsl:stylesheet>