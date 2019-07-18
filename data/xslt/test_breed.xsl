<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:param name="talents" />
<xsl:param name="text" />

<xsl:template match="/breeds/*">
  <xsl:value-of select="$text"/> 
  <xsl:for-each select="document($talents)//group" >
     Name: <xsl:value-of select="@name"/> 
  </xsl:for-each>
  <html>
  <body>
    <h2>My breeds</h2>
    <table border="1">
      <tr bgcolor="#9acd32">
        <th>Title</th>
        <th>Artist</th>
      </tr>
      <tr>
        <td><xsl:value-of select="@name"/></td>
        <td>.</td>
      </tr>
    </table>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
