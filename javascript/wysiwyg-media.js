Drupal.wysiwyg.plugins.media = {

  /**
   * Execute the button.
   */
  invoke: function(data, settings, instanceId) {
    if (data.format == 'html') {
      options = {};
      jQuery().mediaBrowser( function (mediaFiles, options) {
    	// Insert the preview of the file returned into the editor.
        preview = jQuery(mediaFiles[0].preview);
    	jQuery('img',preview).attr('fid',mediaFiles[0].fid);
    	Drupal.wysiwyg.instances[instanceId].insert(Drupal.wysiwyg.plugins.media.addWrapper(preview.html()));
      },options);
    }    
  },
  
  /**
   * Atach function, called when a rich text editor loads.
   * This finds all [[tags]] and replaces them with the html
   * that needs to show in the editor.
   * 
   */
  attach: function(content, settings, instanceId) {
    matches = content.match(/\[\[.*?\]\]/g);
	tagmap = Drupal.settings.tagmap;
	  if(matches) {
	    for (i=0; i< matches.length; i++) {
		  for (var tagContent in tagmap ) {
		    if (tagContent === matches[i]) {
			  // This probably needs some work...
			  // We need to somehow get the fid propogated here.
			  // We really want to
			  matches[i] = matches[i].replace('[[','');
			  matches[i] = matches[i].replace(']]','');
			  mediaObj = JSON.parse(matches[i]);
			  imgMarkup = jQuery(tagmap[tagContent]);
			  jQuery('img',imgMarkup).attr('fid',mediaObj.fid);
			  content = content.replace(tagContent,this.addWrapper(imgMarkup.html()));
			}
		  }
	    }
	  }
	  return content;
  },

  /**
   * Detach function, called when a rich text editor detaches
   */
  detach: function(content, settings, instanceId) {
    var $content = jQuery('<div>' + content + '</div>');
    jQuery('div.media-embedded',$content).each(function (elem){
      var imgNode = jQuery("img",this);
      tagContent = {
        "type": 'media',
    	//@todo: This will be selected from the format form
    	"view_mode": 'media_original',
    	"fid" : imgNode.attr('fid'),
    	"attributes": {
    	  "width" : imgNode.attr('width'),
    	  "height" : imgNode.attr('height'),
    	}
      }
      tagContent = '[[' + JSON.stringify(tagContent) + ']]';
      jQuery(this).replaceWith(tagContent);
    });
    return $content.html();
  },
  
  addWrapper: function(htmlContent) {
	  return '<div class="media-embedded">' + htmlContent + '</div>';  
  },  
};