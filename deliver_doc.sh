#!/bin/sh

directory=doc/build

if [ -z "$RCLONE_CONFIG_OPENPAPERWORK_ACCESS_KEY_ID" ] ; then
  echo "Delivery: No rclone credentials provided."
  exit 0
fi

if ! which rclone; then
  echo "rclone not available."
  exit 1
fi

echo "Delivering: ${directory} (${CI_COMMIT_REF_NAME} - ${CI_COMMIT_SHORT_SHA})"

out_name="$(date "+%Y%m%d_%H%M%S")_${CI_COMMIT_REF_NAME}_${CI_COMMIT_SHORT_SHA}"
latest_name="latest"

if ! rclone --config ./rclone.conf copy "${directory}/" "openpaperwork:openpaperwork-doc/libinsane/${out_name}" ; then
  echo "rclone failed"
  exit 1
fi

if ! rclone --config ./rclone.conf sync "${directory}/" "openpaperwork:openpaperwork-doc/libinsane/latest" ; then
  echo "rclone failed"
  exit 1
fi

echo Success
exit 0
