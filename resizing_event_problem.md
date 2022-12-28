
# The problem is that when we have an resize event we need to change the mWidth and mHeight of App *and* change the aspect ratio of the current camera.

The way it was done before was that the App was in full control of processing SDL_Events. That meant that to change the aspect ratio of the camera, app would need to have a reference to it. That would be weird? Because then there would be a need to attach the Camera to the App, as the Camera was created after the App.

Now main takes care of the polling events from SDL, and calls all processEvent from the classes needed. There is some code duplication, because Camera will check for the same event as App (window resizing) and use the same arguments for similar things. But I think overall it's better.
 - App stays with the width and height of *the window*.
 - Maybe Renderer stays with the width and height of the viewport? Kind like of how opengl works lol.
 - But then window width would useless. Maybe it will. Does it matter though?
 - Extra data I guess.
 - The way it's done now, it's hard to know what is doing what (?) I mean not really.
 - The 'use case' would be me using it to add keybindings, though this would be done by the input controller anyway. I think it's safe to let this way, though decoupling camera from a resizing event seems wrong.
 - I will let the way it is.
