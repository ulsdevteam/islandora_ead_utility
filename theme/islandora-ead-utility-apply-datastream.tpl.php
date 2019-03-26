<?php

/**
 * @file
 * islandora-ead-utility-apply-datastream.tpl display template.
 s*
 * Variables available:
 * - $filename
 * - $file_contents
 * - $subject
 */
?>
<?php if ($has_object): ?>
<b><?php print $filename; ?></b> for object <?php print $Fedora_PID; ?><br>
<div>
<form action="." method="POST">
    <span class="imu_red">Are you sure that you want to apply this
      <?php print $subject; ?> datastream to the underlying object?</span> &nbsp;
    <input type="submit" value="Apply datastream" class="imu_red">
</form>
</div><br>
<?php else: ?>
<b><?php print $filename; ?></b> <i>Object <?php print $Fedora_PID; ?> does not exist.</i><br>

<?php endif; ?>

<textarea class="imu_file_viewer" readonly><?php print $file_contents; ?>
</textarea>