/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

function hsba(h, s, b) {
    var lightness = (2 - s)*b;
    var satHSL = s*b/((lightness <= 1) ? lightness : 2 - lightness);
    lightness /= 2;
    return Qt.hsla(h, satHSL, lightness, 1);
}

function fullColorString(clr) {
    return "#" + ((Math.ceil(255) + 256).toString(16).substr(1, 2) +
            clr.toString().substr(1, 6)).toUpperCase();
}

function getChannelStr(clr, channelIdx) {
    return parseInt(clr.toString().substr(channelIdx*2 + 1, 2), 16);
}

function getHsbComponent(r, g, b, componentIdx) {
    var maximum = Math.max(r, g, b);
    var minimum = Math.min(r, g, b);
    r = parseInt(r);
    g = parseInt(g);
    b = parseInt(b);

    //hue
    if (componentIdx === 0) {
        if (maximum === minimum) {
           return 0;
        }

        else if (maximum == r) {
            if (g > b) {
                return 60 * ((g - b) / (maximum - minimum));
            }

            else {
                return 60 * ((g - b) / (maximum - minimum)) + 360;
            }
        }

        else if (maximum == g) {
            return 60 * ((b - r) / (maximum - minimum)) + 120;
        }

        else if(maximum == b) {
            return 60 * ((r - g) / (maximum - minimum)) + 240;
        }        
    }

    //saturation
    else if(componentIdx === 1) {
        if (maximum === 0) {
            return 0;
        }

        else {
            return 1 - minimum / maximum;
        }
    }

    //brightness
    else if(componentIdx === 2) {
        return maximum/255;
    }
}
