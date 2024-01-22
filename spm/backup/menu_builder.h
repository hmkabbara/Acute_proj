/*
 * menu_builder.h
 *
 *  Created on: December, 17, 2023
 *      Author: hazem Kabbara
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 */

#ifndef MENU_BUILDER_H_
#define MENU_BUILDER_H_


/*
 * Public directories that applications can register with.
 * Enable define for these root directories.  Once defined,
 * apps can register in this directory structure.  If not,
 * applications register to the root directory.
 */
#define CLI_ROOT_DIRS     ( 1 )

#define MIN_VAL					-40.00
#define MAX_VAL  				99.00

char menu_build (void);

#endif /* MENU_BUILDER_H_ */
