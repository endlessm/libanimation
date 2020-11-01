/*
 * /tests/js/extension.js
 *
 * Copyright (C) 2019 Sam Spilsbury.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Shell extension which implements libanimation to provide nice window
 * animations.
 */

const Animation = imports.gi.Animation;
const AnimationClutter = imports.gi.AnimationClutter;
const Clutter = imports.gi.Clutter;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Meta = imports.gi.Meta;
const Main = imports.ui.main;

const settings = new Gio.Settings({
    schema: 'org.gnome.shell.extensions.animation'
});

const BoxWrapper = GObject.registerClass({
    Properties: {
        'box': GObject.ParamSpec.boxed('box',
                                       'AnimationBox',
                                       'AnimationBox to animate to (static)',
                                       GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                                       Animation.Box)
    }
}, class BoxWrapper extends Animation.BoxQuery {
    _init(props) {
        super._init(props);
        this.update(this.box);
    }
});

const bindObjectPropsToSettings = (animation, props, settings) => {
    Object.keys(props).forEach(key => {
        settings.bind(props[key],
                      animation,
                      key,
                      Gio.SettingsBindFlags.GET)
    });

    return animation;
};

const ZoomAnimation = GObject.registerClass({
}, class ZoomAnimation extends AnimationClutter.AffineEffect {
    constructor(props) {
        super(props);
        this._enabledId = 0;
    }

    _startAnimation(animation, done) {
        this.transform_animation = animation;
        this.enabled = true;

        this._enabledId = this.connect('notify::enabled', () => {
            if (!this.enabled) {
                done();
                this.disconnect(this._enabledId);
                this._enabledId = 0;
            }
        });
    }

    activate(event, detail, done) {
        let [x, y] = this.actor.get_position();
        let [width, height] = this.actor.get_size();

        switch(event) {
            case 'open':
                this._startAnimation(new Animation.ZoomAnimation({
                    source: new AnimationClutter.ActorBoxQuery({
                        actor: this.actor
                    }),
                    target: new BoxWrapper({
                        box: new Animation.Box({
                            top_left: new Animation.Vector({
                                x: detail.pointer[0],
                                y: detail.pointer[1]
                            }),
                            bottom_right: new Animation.Vector({
                                x: detail.pointer[0],
                                y: detail.pointer[1]
                            })
                        })
                    }),
                    stepper: bindObjectPropsToSettings(new Animation.LinearStepper(), {
                        length: 'zoom-length'
                    }, settings)
                }), done);
                return true;
            case 'close':
                this._startAnimation(new Animation.ZoomAnimation({
                    source: new BoxWrapper({
                        box: new Animation.Box({
                            top_left: new Animation.Vector({
                                x: detail.pointer[0],
                                y: detail.pointer[1]
                            }),
                            bottom_right: new Animation.Vector({
                                x: detail.pointer[0],
                                y: detail.pointer[1]
                            })
                        })
                    }),
                    target: new AnimationClutter.ActorBoxQuery({
                        actor: this.actor
                    }),
                    stepper: new Animation.ReverseStepper({
                        base_stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), {
                            length: 'zoom-length'
                        }, settings)
                    })
                }), done);
                return true;
            case 'minimize':
                this._startAnimation(new Animation.ZoomAnimation({
                    source: new BoxWrapper({
                        box: new Animation.Box({
                            top_left: new Animation.Vector({
                                x: detail.icon.x,
                                y: detail.icon.y
                            }),
                            bottom_right: new Animation.Vector({
                                x: detail.icon.x + detail.icon.width,
                                y: detail.icon.y + detail.icon.height
                            })
                        })
                    }),
                    target: new AnimationClutter.ActorBoxQuery({
                        actor: this.actor
                    }),
                    stepper: new Animation.ReverseStepper({
                        base_stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), {
                            length: 'zoom-length'
                        }, settings)
                    })
                }), done);
                return true;
            case 'unminimize':
                this._startAnimation(new Animation.ZoomAnimation({
                    target: new AnimationClutter.ActorBoxQuery({
                        actor: this.actor
                    }),
                    source: new BoxWrapper({
                        box: new Animation.Box({
                            top_left: new Animation.Vector({
                                x: detail.icon.x,
                                y: detail.icon.y
                            }),
                            bottom_right: new Animation.Vector({
                                x: detail.icon.x + detail.icon.width,
                                y: detail.icon.y + detail.icon.height
                            })
                        })
                    }),
                    stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), {
                        length: 'zoom-length'
                    }, settings)
                }), done);
                return true;
            default:
                return false;
        }
    }

    remove() {
        if (this.actor) {
            this.actor.remove_effect(this);
        }

        this.set_enabled(false);
    }
});

const BounceAnimation = GObject.registerClass({
}, class BounceAnimation extends AnimationClutter.AffineEffect {
    constructor(params) {
        super(params);
        this._enabledId = 0;
    }

    _startAnimation(animation, done) {
        this.transform_animation = animation;
        this.enabled = true;

        this._enabledId = this.connect('notify::enabled', () => {
            if (!this.enabled) {
                done();
                this.disconnect(this._enabledId);
                this._enabledId = 0;
            }
        });
    }

    activate(event, detail, done) {
        let [x, y] = this.actor.get_position();
        let [width, height] = this.actor.get_size();

        switch (event) {
            case 'open':
                this._startAnimation(bindObjectPropsToSettings(new Animation.BounceAnimation({
                    target: new AnimationClutter.ActorBoxQuery({
                        actor: this.actor
                    }),
                    stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), {
                        length: 'bounce-length'
                    }, settings)
                }), {
                    initial_scale: 'bounce-initial-scale',
                    maximum_scale: 'bounce-maximum-scale',
                    n_bounce: 'bounce-n-bounce',
                }, settings), done);
                return true;
            case 'close':
                this._startAnimation(bindObjectPropsToSettings(new Animation.BounceAnimation({
                    target: new AnimationClutter.ActorBoxQuery({
                        actor: this.actor
                    }),
                    stepper: new Animation.ReverseStepper({
                        base_stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), {
                            length: 'bounce-length'
                        }, settings)
                    })
                }), {
                    initial_scale: 'bounce-initial-scale',
                    maximum_scale: 'bounce-maximum-scale',
                    n_bounce: 'bounce-n-bounce',
                }, settings), done);
                return true;
            default:
                return false;
        }
    }

    remove() {
        if (this.actor) {
            this.actor.remove_effect(this);
        }

        this.set_enabled(false);
    }
});

 // A boune animation as attached to an actor.
const GlideAnimation = GObject.registerClass({
}, class GlideAnimation extends AnimationClutter.AffineEffect {
    constructor(params) {
        super(params);
        this._enabledId = 0;
    }

    _startAnimation(animation, done) {
        this.transform_animation = animation;
        this.enabled = true;

        this._enabledId = this.connect('notify::enabled', () => {
            if (!this.enabled) {
                done();
                this.disconnect(this._enabledId);
                this._enabledId = 0;
            }
        });
    }

    activate(event, detail, done) {
        let [x, y] = this.actor.get_position();
        let [width, height] = this.actor.get_size();

        switch (event) {
            case 'open':
                this._startAnimation(bindObjectPropsToSettings(new Animation.GlideAnimation({
                    viewport: new AnimationClutter.ActorBoxQuery({
                        actor: global.get_stage()
                    }),
                    target: new AnimationClutter.ActorBoxQuery({
                        actor: this.actor
                    }),
                    stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), { 
                        length: 'glide-length'
                    }, settings)
                }), {
                    x_rotation_angle_degrees: 'glide-x-rotation-angle-degrees',
                    y_rotation_angle_degrees: 'glide-y-rotation-angle-degrees',
                    x_axis_location_unit: 'glide-x-axis-location-unit',
                    y_axis_location_unit: 'glide-y-axis-location-unit',
                }, settings), done);
                return true;
            case 'close':
                this._startAnimation(bindObjectPropsToSettings(new Animation.GlideAnimation({
                    viewport: new AnimationClutter.ActorBoxQuery({
                        actor: global.get_stage()
                    }),
                    target: new AnimationClutter.ActorBoxQuery({
                        actor: this.actor
                    }),
                    stepper: new Animation.ReverseStepper({
                        base_stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), { 
                            length: 'glide-length'
                        }, settings)
                    })
                }), {
                    x_rotation_angle_degrees: 'glide-x-rotation-angle-degrees',
                    y_rotation_angle_degrees: 'glide-y-rotation-angle-degrees',
                    x_axis_location_unit: 'glide-x-axis-location-unit',
                    y_axis_location_unit: 'glide-y-axis-location-unit',
                }, settings), done);
                return true;
            default:
                return false;
        }
    }

    remove() {
        if (this.actor) {
            this.actor.remove_effect(this);
        }
    }
});

const MetaWindowOffsetActorBoxQuery = GObject.registerClass({
    Properties: {
        'actor': GObject.ParamSpec.object('actor',
                                          'ClutterActor',
                                          'The ClutterActor',
                                          GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                                          Clutter.Actor),
        'window': GObject.ParamSpec.object('window',
                                           'MetaWindow',
                                           'The MetaWindow',
                                           GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                                           Meta.Window)
    }
}, class MetaWindowOffsetActorBoxQuery extends Animation.BoxQuery {
    _init(params) {
        super._init(params);

        this.actor.connect('notify::x', () => this._updateGeometry());
        this.actor.connect('notify::y', () => this._updateGeometry());
        this.actor.connect('notify::width', () => this._updateGeometry());
        this.actor.connect('notify::height', () => this._updateGeometry());

        this.window.connect('position-changed', () => this._updateGeometry());
        this.window.connect('size-changed', () => this._updateGeometry());

        this._updateGeometry();
    }

    _updateGeometry() {
        let [x, y] = this.actor.get_position();
        let [width, height] = this.actor.get_size();
        let actorGeometry = this.actor.get_meta_window().get_frame_rect();

        // Need to know the difference between the window size with shadows
        // and without shadows in order to offset the window box correctly.
        let offsetX = Math.ceil((width - actorGeometry.width) / 2);
        let offsetY = Math.ceil((height - actorGeometry.height) / 2);

        let box = {
            x1: x - offsetX,
            y1: y - offsetY,
            x2: x - offsetX + width,
            y2: y - offsetY + height
        };

        this.update(new Animation.Box({
            top_left: new Animation.Vector({ x: box.x1, y: box.y1 }),
            bottom_right: new Animation.Vector({ x: box.x2, y: box.y2 })
        }));
    }
});

// MagicLampAnimation
//
// A magic lamp animation as attached to an actor.
const MagicLampAnimation = GObject.registerClass({
}, class MagicLampAnimation extends AnimationClutter.GridEffect {
    constructor(params) {
        super(params);
        this._enabledId = 0;
    }

    _startAnimation(animation, done) {
        this.grid_animation = animation;
        this.x_tiles = 2;
        this.y_tiles = 100;
        this.enabled = true;

        this._enabledId = this.connect('notify::enabled', () => {
            if (!this.enabled) {
                done();
                this.disconnect(this._enabledId);
                this._enabledId = 0;
            }
        });
    }

    activate(event, detail, done) {
        let [x, y] = this.actor.get_position();
        let [width, height] = this.actor.get_size();
        let actorGeometry = this.actor.get_meta_window().get_frame_rect();

        // Need to know the difference between the window size with shadows
        // and without shadows in order to offset the window box correctly.
        let offsetX = Math.ceil((width - actorGeometry.width) / 2);
        let offsetY = Math.ceil((height - actorGeometry.height) / 2);

        let box = {
            x1: x - offsetX,
            y1: y - offsetY,
            x2: x - offsetX + width,
            y2: y - offsetY + height
        };

        switch (event) {
            case 'minimize':
                this._startAnimation(bindObjectPropsToSettings(new Animation.MagicLampAnimation({
                    source: new BoxWrapper({
                        box: new Animation.Box({
                            top_left: new Animation.Vector({
                                x: detail.icon.x,
                                y: detail.icon.y
                            }),
                            bottom_right: new Animation.Vector({
                                x: detail.icon.x + detail.icon.width,
                                y: detail.icon.y + detail.icon.height
                            })
                        })
                    }),
                    target: new MetaWindowOffsetActorBoxQuery({
                        actor: this.actor,
                        window: this.actor.get_meta_window()
                    }),
                    resolution: new Animation.Vector({ x: 2, y: 100 }),
                    stepper: new Animation.ReverseStepper({
                        base_stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), {
                            length: 'magiclamp-length'
                        }, settings)
                    })
                }), {
                    bend_factor: 'magiclamp-bend-factor',
                    offset_factor: 'magiclamp-offset-factor',
                    stretch_factor: 'magiclamp-stretch-factor',
                    deform_speed_factor: 'magiclamp-deform-speed-factor',
                }, settings), done);
                return true;
            case 'unminimize':
                this._startAnimation(bindObjectPropsToSettings(new Animation.MagicLampAnimation({
                    source: new BoxWrapper({
                        box: new Animation.Box({
                            top_left: new Animation.Vector({
                                x: detail.icon.x,
                                y: detail.icon.y
                            }),
                            bottom_right: new Animation.Vector({
                                x: detail.icon.x + detail.icon.width,
                                y: detail.icon.y + detail.icon.height
                            })
                        })
                    }),
                    target: new MetaWindowOffsetActorBoxQuery({
                        actor: this.actor,
                        window: this.actor.get_meta_window()
                    }),
                    resolution: new Animation.Vector({ x: 2, y: 100 }),
                    stepper: bindObjectPropsToSettings(new Animation.LinearStepper({}), {
                        length: 'magiclamp-length'
                    }, settings)
                }), {
                    bend_factor: 'magiclamp-bend-factor',
                    offset_factor: 'magiclamp-offset-factor',
                    stretch_factor: 'magiclamp-stretch-factor',
                    deform_speed_factor: 'magiclamp-deform-speed-factor',
                }, settings), done);
                return true;
            default:
                return false;
        }
    }

    remove() {
        if (this.actor) {
            this.actor.remove_effect(this);
        }

        this.set_enabled(false);
    }
});

const WobblyEffect = GObject.registerClass({
}, class WobblyEffect extends AnimationClutter.WobblyEffect {
    constructor(params) {
        super(params);
        bindObjectPropertiesToSettings(this, {
            object_movement_range: 'wobbly-object-movement-range',
            spring_k: 'wobbly-spring-k',
            friction: 'wobbly-friction',
            slowdown_factor: 'wobbly-slowdown-factor'
        }, settings);
    }

    activate(event, detail) {
        switch (event) {
            case 'move':
                detail.grabbed ? this._grabbedByMouse() : this._ungrabbedByMouse();
                return true;
            default:
                return false;
        }
    }

    remove() {
        if (this.actor) {
            this.actor.remove_effect(this);
            this.actor = null;
        }
    }

    _grabbedByMouse() {
        let position = global.get_pointer();
        let actor = this.get_actor();
        this.grab(position[0], position[1]);

        this._lastPosition = actor.get_position();
        this._positionChangedId =
            actor.connect('allocation-changed', (actor) => {
                let position = actor.get_position();
                let dx = position[0] - this._lastPosition[0];
                let dy = position[1] - this._lastPosition[1];

                this.move_by(dx, dy);
                this._lastPosition = position;
            });
    }

    _ungrabbedByMouse() {
        // Only continue if we have an active grab and change notification
        // on movement
        if (!this._positionChangedId)
            return;

        let actor = this.get_actor();
        this.ungrab();

        actor.disconnect(this._positionChangedId);
        this._positionChangedId = null;
    }
});


function init() {
}

let connections = {
};

function disable() {
    Object.keys(connections).forEach(k => {
        let [signal, object] = connections[k];
        object.disconnect(signal);
        delete connections[k];
    });
}

const ANIMATION_NAME_TO_CLASS = {
    'bounce': BounceAnimation,
    'zoom': ZoomAnimation,
    'magiclamp': MagicLampAnimation,
    'glide': GlideAnimation,
}

const DEFAULT_EFFECTS = {
    'open': 'glide',
    'close': 'glide',
    'minimize': 'zoom',
    'unminimize': 'zoom',
    'move': 'none'
}

function createAnimationForEvent(eventName) {
    let effectName = settings.get_string(`${eventName}-effect`);

    if (effectName === 'default') {
        effectName = DEFAULT_EFFECTS[eventName];
    }

    return [new ANIMATION_NAME_TO_CLASS[effectName]({}), effectName];
}

function enable() {
    let wm = global.window_manager;
    connections['minimize'] = [wm.connect_after('minimize', (shellwm, actor) => {
        let [hasIcon, icon] = actor.meta_window.get_icon_geometry();

        if (!hasIcon)
            return;

        // Remove all existing tweens and minimize animations first first
        if (Main.wm._removeEffect(Main.wm._minimizing, actor)) {
            actor.remove_all_transitions();
            actor.set_pivot_point(0, 0);
            actor.scale_y = 1;
            actor.scale_x = 1;
            actor.translation_y = 0;
            actor.translation_x = 0;
            actor.opacity = 255;
        }

        // Use libanimation to run an animation
        let [animation, effectName] = createAnimationForEvent('minimize');
        actor.add_effect_with_name(`animation::${effectName}`, animation);
        animation.activate('minimize', {
            // Guaranteed to work, since the minimize effect is
            // only available if this window has an icon
            icon: icon,
        }, () => {
            shellwm.completed_minimize(actor);
            actor.remove_effect(animation);
        });
    }), wm];
    connections['unminimize'] = [wm.connect_after('unminimize', (shellwm, actor) => {
        let [hasIcon, icon] = actor.meta_window.get_icon_geometry();

        if (!hasIcon)
            return;

        // Remove all existing tweens and minimize animations first first
        if (Main.wm._removeEffect(Main.wm._unminimizing, actor)) {
            actor.remove_all_transitions();
            actor.set_pivot_point(0, 0);
            actor.scale_y = 1;
            actor.scale_x = 1;
            actor.translation_y = 0;
            actor.translation_x = 0;
            actor.opacity = 255;

            // Also undo actor translation and return to frame rect
            let rect = actor.meta_window.get_frame_rect();
            actor.set_position(rect.x, rect.y);
        }

        // Use libanimation to run an animation
        let [animation, effectName] = createAnimationForEvent('unminimize');
        actor.add_effect_with_name(`animation::${effectName}`, animation);
        animation.activate('unminimize', {
            // Guaranteed to work, since the minimize effect is
            // only available if this window has an icon
            icon: icon,
        }, () => {
            shellwm.completed_unminimize(actor);
            actor.remove_effect(animation);
        });
    }), wm];
    connections['map'] = [wm.connect_after('map', (shellwm, actor) => {
        let types = [Meta.WindowType.NORMAL,
                     Meta.WindowType.DIALOG,
                     Meta.WindowType.MODAL_DIALOG];
        if (!Main.wm._shouldAnimateActor(actor, types) || actor._windowType != Meta.WindowType.NORMAL) {
            // We weren't going to animate this actor anyway
            return;
        }

        // Remove all existing tweens and open animations first
        if (Main.wm._removeEffect(Main.wm._mapping, actor)) {
            actor.remove_all_transitions();
            actor.set_pivot_point(0, 0);
            actor.scale_y = 1;
            actor.scale_x = 1;
            actor.translation_y = 0;
            actor.translation_x = 0;
            actor.opacity = 0;
        }

        // Use libanimation to run an animation
        let [animation, effectName] = createAnimationForEvent('open');
        actor.add_effect_with_name(`animation::${effectName}`, animation);
        animation.activate('open', {}, () => {
            shellwm.completed_map(actor);
            actor.remove_effect(animation);
        });
    }), 'wm'];
    connections['destroy'] = [wm.connect_after('destroy', (shellwm, actor) => {
        let types = [Meta.WindowType.NORMAL,
                     Meta.WindowType.DIALOG,
                     Meta.WindowType.MODAL_DIALOG];
        if (!Main.wm._shouldAnimateActor(actor, types) ||
            (actor._windowType && actor._windowType != Meta.WindowType.NORMAL)) {
            // We weren't going to animate this actor anyway
            return;
        }

        // Remove all existing tweens and destroy animations first
        if (Main.wm._removeEffect(Main.wm._destroying, actor)) {
            actor.remove_all_transitions();
            actor.set_pivot_point(0, 0);
            actor.scale_y = 1;
            actor.scale_x = 1;
            actor.translation_y = 0;
            actor.translation_x = 0;
            actor.opacity = 255;
        }

        // Use libanimation to run an animation
        let [animation, effectName] = createAnimationForEvent('close');
        actor.add_effect_with_name(`animation::${effectName}`, animation);
        animation.activate('close', {}, () => {
            shellwm.completed_destroy(actor);
            // Can't remove the effect since the actor is already gone
        });
    }), wm];

    connections['grab-op-begin'] = [global.display.connect('grab-op-begin', (display, screen, window, op) => {
        // Occassionally, window can be null, in cases where grab-op-begin
        // was emitted on a window from shell-toolkit. Ignore these grabs.
        if (!window)
            return;

        if (window.is_override_redirect() ||
            op != Meta.GrabOp.MOVING)
            return;

        // Right now we only have the wobbly effect for moving windows,
        // so we just check directly if we were enabled.
        if (settings.get_string('move-effect') !== 'wobbly')
            return;

        let actor = window.get_compositor_private();

        log(`Grab ${actor.x} ${actor.y} ${actor.width} ${actor.height}`);

        if (!actor._wobblyEffect) {
            actor._wobblyEffect = new WobblyEffect({});
            actor.add_effect_with_name('animation::wobbly', actor._wobblyEffect);
        }

        actor._wobblyEffect.activate('move', {
            grabbed: true
        });
    }), global.display];

    connections['grab-op-end'] = [global.display.connect('grab-op-end', (display, screen, window, op) => {
        // Occassionally, window can be null, in cases where grab-op-begin
        // was emitted on a window from shell-toolkit. Ignore these grabs.
        if (!window)
            return;

        let actor = window.get_compositor_private();

        if (!actor._wobblyEffect) {
            return;
        }

        actor._wobblyEffect.activate('move', {
            grabbed: false
        });
    }), global.display];
}
