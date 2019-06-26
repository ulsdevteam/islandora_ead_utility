<?php

/**
 * @file
 * islandora-ead-utility-apply-datastream.tpl display template.
 s*
 * Variables available:
 * - $filename
 * - $file_contents
 */
?>
<?php if ($has_object): ?>
<b><?php print $filename; ?></b> for object <?php print $Fedora_PID; ?><br>
<?php else: ?>
<b><?php print $filename; ?></b> <i>Object <?php print $Fedora_PID; ?> does not exist.</i><br>
<?php endif; ?>

<?php if ($file_contents) : ?>
<textarea class="imu_file_viewer" readonly><?php print $file_contents; ?>
</textarea>
<?php endif; ?>